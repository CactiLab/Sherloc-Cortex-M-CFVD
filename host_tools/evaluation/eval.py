#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311

import os
import shutil
import xml.etree.ElementTree as ET
import subprocess
import sys
import copy
import serial
import time
from datetime import datetime
import ctypes

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from capstone import Cs, CsError, CS_ARCH_ARM, CS_MODE_THUMB, arm, CS_OPT_SYNTAX_ATT
from capstone import *
import json
import struct
from pathlib import Path
from bitarray import bitarray
import pandas as pd

from ibt_analysis import *

"""_summary_
    auto the evaluation
Input:
    benchmark name
Output:
    executable .axf file 
    evaluation results
"""

INS_TAGGING = 0
TEST_ALL = 1
base = 0x200000
CODE_NS = 0x400000


def write_log(path, log, file_type):
    try:
        with open(path, file_type) as file:
            file.write(log)
    except FileNotFoundError:
        print("The file does not exist.")
        sys.exit()
    except PermissionError:
        print("You do not have permission to write to this file. " + path)
        sys.exit()
    except IsADirectoryError:
        print("You attempted to open a directory as a file. " + path)
        sys.exit()
    except IOError as e:
        print(f"An I/O error occurred: {str(e)}")
        sys.exit()


def string_int(addr):
    return int(hex(addr), 16)


def concat_32(src, dst):
    return hex((src - base) << 16 | (dst - base)) if dst > 0 else hex((src - base) << 16 | dst)


def check_and_create_directory(directory_name):
    if os.path.exists(directory_name):
        print(f'{directory_name} exists, return.')
        return
    else:
        os.mkdir(directory_name)


def check_and_create_file(file_name):
    if os.path.exists(file_name):
        print(f'{file_name} exists, return.')
        return
    else:
        try:
            f = open(file_name, 'x')
            f.close()
        except FileExistsError:
            print("The file already exists.")


def copy_file(src_file, dst_file):
    if os.path.exists(src_file):
        shutil.copy(src_file, dst_file)
    else:
        print(f'{src_file} does not exist.')
        sys.exit()


def move_file(src_file, dst_file):
    if os.path.exists(src_file):
        shutil.move(src_file, dst_file)
        # shutil.copy(src_file, dst_file)
    else:
        print(f'{src_file} does not exist.')
        sys.exit()


def get_timestamp():
    # Get the current time using time.time()
    current_time = time.time()

    # Convert the current time to a datetime object
    dt_object = datetime.fromtimestamp(current_time)

    # Extract and display the year, month, day, hour, and second
    year = dt_object.year
    month = dt_object.month
    day = dt_object.day
    hour = dt_object.hour
    second = dt_object.second

    return (f'{year}_{month}_{day}_{hour}_{second}')


class BtHeader(ctypes.Structure):
    _fields_ = [
        ("dwMagicCode", ctypes.c_uint32),   # header, __MGAIC_CODE__
        ("IBT_size", ctypes.c_int32),
        ("IRQ_size", ctypes.c_int32),
        ("TASK_size", ctypes.c_int32),
        ("LOOP_size", ctypes.c_int32),
        ("INS_tag_size", ctypes.c_int32),
        ("PendSV", ctypes.c_uint32),
        ("IBT_entry", ctypes.POINTER(ctypes.c_uint32)),
        ("IRQ_entry", ctypes.POINTER(ctypes.c_uint32)),
        ("TASK_entry", ctypes.POINTER(ctypes.c_uint32)),
        ("LOOP_entry", ctypes.POINTER(ctypes.c_uint32)),
        ("INS_tag", ctypes.POINTER(ctypes.c_uint8)),
    ]


class MetaDataRefresh(object):
    def __init__(self, out_eval_log='', bin_file_path='', new_record=''):
        self.out_eval_log = out_eval_log
        self.bin_file_path = bin_file_path
        self.new_record = new_record

    def parse_metadata_from_file(self):
        with open(self.bin_file_path, "rb") as f:
            # Read the binary data from the file
            bin_data = f.read()

        # Create a buffer using the binary data and cast it to the BtHeader structure
        bt_header = ctypes.cast(ctypes.create_string_buffer(
            bin_data), ctypes.POINTER(BtHeader)).contents
        rest_bin_data = bin_data[48:]
        IBT_data_pos = bt_header.IBT_size*4
        IBT_data = rest_bin_data[:IBT_data_pos]
        other_data = rest_bin_data[IBT_data_pos:]

        return bt_header, IBT_data, other_data, bin_data

    def bt_header_to_bytes(self, bt_header):
        return bytes(ctypes.string_at(ctypes.pointer(bt_header), 48))

    def insert_sort_bin(self):
        write_log(self.out_eval_log,
                  f'Inserting IBT entry: {self.new_record}\n', 'a+')

        # Convert y to an integer
        new_record_int = int(self.new_record[2:].decode(), 16)

        # Convert new_record_int to a little-endian bytearray
        new_record_little_endian = bytearray(
            new_record_int.to_bytes(4, 'little'))

        # new_record_little_endian = self.new_record

        # Concatenate x and new_record_little_endian
        if self.IBT_data.find(new_record_little_endian) == -1:
            self.bt_header.IBT_size += 1
            self.IBT_data = self.IBT_data + new_record_little_endian

        # Convert x to a list of little-endian integers
        num_integers = len(self.IBT_data) // 4
        write_log(self.out_eval_log,
                  f'count of IBT_data: {num_integers}\n', 'a+')
        integers = struct.unpack(f'<{num_integers}I', self.IBT_data)

        # Sort the list of integers
        sorted_integers = sorted(integers)

        # Convert the sorted list back to a little-endian bytearray
        self.IBT_data = struct.pack(f'<{num_integers}I', *sorted_integers)

    def restore_bin_file(self):
        modified_bin_data = self.bt_header_to_bytes(
            self.bt_header) + self.IBT_data + self.other_data
        # Write the modified binary data back to the file
        with open(self.bin_file_path, 'wb') as f:
            f.write(modified_bin_data)

    def clean_ibt(self):
        # walk through the ibt list, delete the item that the last 16 bits are zeros. the item is stored as a 32-bit little endian integer.
        # Convert x to a list of little-endian integers
        num_integers = len(self.IBT_data) // 4
        write_log(self.out_eval_log,
                  f'count of IBT_data: {num_integers}\n', 'a+')
        integers = struct.unpack(f'<{num_integers}I', self.IBT_data)
        # Sort the list of integers
        sorted_integers = sorted(integers)
        # Convert the sorted list back to a little-endian bytearray
        self.IBT_data = struct.pack(f'<{num_integers}I', *sorted_integers)
        write_log(self.out_eval_log,
                  f'sorted IBT_data: {self.IBT_data}\n', 'a+')

    def refresh_run(self):
        self.bt_header, self.IBT_data, self.other_data, self.bin_data = self.parse_metadata_from_file()
        self.insert_sort_bin()
        self.restore_bin_file()


class BoardOperation(object):
    def __init__(self, opt, out_eval_log, src_path, dst_path, hardware_reset_port, software_reset_port):
        self.opt = opt
        self.out_eval_log = out_eval_log
        self.hardware_reset_port = hardware_reset_port
        self.software_reset_port = software_reset_port
        self.src_path = src_path
        self.dst_path = dst_path

    def HardwareReset_MPS2(self):
        restPort = serial.Serial(self.hardware_reset_port, 115200)
        restPort.write(b"\x00")
        restPort.close()
        print('reset board success. sleep(2)\n')
        write_log(self.out_eval_log, "reset board success\n\n", 'a+')
        time.sleep(2)

    def SoftwareReset_MPS2(self):
        OnPort = serial.Serial(self.software_reset_port, 115200)
        OnPort.write(b"\x00")
        OnPort.close()
        print('on board success. sleep(2)\n')
        write_log(self.out_eval_log, "on board success\n", 'a+')

    def load_file(self):
        if os.path.exists(self.dst_path):
            write_log(self.out_eval_log, "delete old " +
                      self.dst_path + '.\n', 'a+')
            os.remove(self.dst_path)
        if os.path.exists(self.src_path):
            try:
                shutil.copy(self.src_path, self.dst_path)
            except Exception as e:
                # hardware reset the board
                BoardOperation('hw', self.out_eval_log, '', '', self.hardware_reset_port,
                               self.software_reset_port).run_board_operation()
            write_log(self.out_eval_log, self.src_path +
                      " loading success.\n", 'a+')
        else:
            write_log(self.out_eval_log, self.src_path +
                      " does not exist. hardware reset the board.\n", 'a+')
            BoardOperation('hw', self.out_eval_log, '', '', self.hardware_reset_port,
                           self.software_reset_port).run_board_operation()
            return

    def run_board_operation(self):
        if self.opt == 'sw':
            self.SoftwareReset_MPS2()
        if self.opt == 'hw':
            self.HardwareReset_MPS2()


class UVPROJXBuilderByName(object):
    def __init__(self, proj_name, in_proj_path, out_binary_ns_path, out_tmp_path, out_binary_name):
        self.proj_name = proj_name
        self.out_binary_name = out_binary_name
        self.in_proj_path = in_proj_path
        self.proj_uvprojx = in_proj_path + proj_name + '.uvprojx'
        self.build_obj = in_proj_path + 'Objects/'
        self.object = self.build_obj + proj_name + '.axf'
        self.object_dump = self.build_obj + proj_name + '.dump'
        self.build_file = in_proj_path + proj_name + '_build.log'

        self.out_binary = out_binary_ns_path + out_binary_name + '.axf'
        self.out_tmp_path = out_tmp_path
        self.out_log = out_tmp_path + out_binary_name + '.log'

    def builder_by_name_run(self):
        build_log_name = self.proj_name + "_build.log"
        build_cmd = "C:/Keil_v5/UV4/UV4.exe -j4 -b " + \
            self.proj_uvprojx + " -o " + build_log_name
        dump_cmd = "arm-none-eabi-objdump.exe -d " + \
            self.object + " > " + self.object_dump

        print(build_cmd)
        process = subprocess.Popen(
            build_cmd, shell=True, stdout=subprocess.PIPE)
        process.wait()
        print("build process return code: ", process.returncode)

        if os.path.exists(self.build_file):
            shutil.move(self.build_file, self.out_log)
            with open(self.out_log, "r+") as f:
                for line in f:
                    if "Error(s)" in line:
                        if "0 Error(s)" in line:
                            print("build project success")
                            shutil.copy(self.object, self.out_binary)
                            os.system(dump_cmd)
                        else:
                            print(
                                self.proj_name, "build failure, please check the build log file...")
                            sys.exit()
        else:
            print(f'{self.build_file} does not exist!')
            sys.exit()


class MetadataCreatorByName(object):
    def __init__(self, filepath, out_metadata_path, out_log_path):
        self.filename = Path(filepath).stem
        self.filepath = filepath
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.out_static_analysis = out_log_path + 'static_analysis.log'
        self.base = 0x200000
        self.ins_tag = bitarray()
        self.branch_tab_32 = self.get_ibt()

        self.code_data, self.symtab, self.task_json, self.section_info, self.code_start, self.code_end = self.process_elf()

        self.sym_list, self.IRQ_entry, self.handler_count, self.task_entry, self.PendSV = self.process_symbols()

        self.icall_count, self.icall_list, self.ibranch_count, self.ibranch_list, self.dcall_count, self.dcall_list, self.ret_count, self.ret_list, self.dbranch_list, self.loop_count, self.loop_entry = self.metadata_analysis_all_4()

        self.sorted_branch, self.sorted_IRQ_entry, self.sorted_task_entry, self.sorted_loop_entry, self.final_icall_list, self.final_ibranch_list, self.final_dcall_list, self.final_ret_list, self.final_dbranch_list = self.sort_records()

    """
    code_data_tmp["start"] = code_ns_unprivileged['sh_addr']
    code_data_tmp["end"] = code_ns_unprivileged['sh_addr'] + code_ns_unprivileged['sh_size']
    """

    def get_data_section(self, tmp_offset, tmp_end):
        for i in self.code_data:
            start = self.code_data[i]['sh_addr']
            end = self.code_data[i]['sh_addr'] + self.code_data[i]['sh_size']

            if tmp_offset >= start and tmp_offset <= end:
                tmp_offset = tmp_offset - start
                tmp_end = tmp_end - start

                return self.code_data[i].data()[tmp_offset:tmp_end]
        return ""

    '''
    @
    load task json file to analyze the symbol with address pairs
    @
    '''

    def process_elf(self):
        # load task json file to identify task start address
        try:
            f = open('./rtos/task.json', )
        except IOError as e:
            print(e)
        # returns JSON object as a dictionary
        try:
            task_json = json.load(f)
        except ValueError:
            print("load task json file error.")
        f.close()
        # open file
        try:
            target_file = open(self.filepath, 'rb')
        except IOError as e:
            print(e)

        elf = ELFFile(target_file)

        write_log(self.out_static_analysis,
                  f'Processing file: {self.filepath}', 'a+')

        code_data = {}
        section_info = {}
        section_tmp = {}
        code_start = 0
        code_end = 0
        # read sections
        write_log(self.out_static_analysis,
                  "\n-------------- Section scatter --------------", 'a+')
        for section in elf.iter_sections():
            section_tmp["start"] = section['sh_addr']
            section_tmp["size"] = section['sh_size']
            section_info[section.name] = copy.copy(section_tmp)
            write_log(self.out_static_analysis,
                      f"{hex(section['sh_addr'])}, {section.name}, {section['sh_size']}\n", 'a+')

        # walk .text section for normal elf file
        code_section = elf.get_section_by_name('.text')
        if code_section:
            code_data[0] = copy.copy(code_section)
            code_start = copy.copy(section_info[".text"])
            code_end = copy.copy(code_start + section_info[".text"]["size"])

        # For bare-metal and FreeRTOS
        else:
            code_section = elf.get_section_by_name('ER_ROM')

        if code_section:
            code_data[0] = copy.copy(code_section)
            code_start = copy.copy(section_info["ER_ROM"]["start"])
            code_end = copy.copy(code_start + section_info["ER_ROM"]["size"])
            pass
        else:  # For FreeRTOS+MPU
            code_data[0] = elf.get_section_by_name('ER_IROM_NS_PRIVILEGED')
            code_data[1] = elf.get_section_by_name(
                'ER_IROM_NS_FREERTOS_SYSCALL')
            code_data[2] = elf.get_section_by_name('ER_IROM_NS_UNPRIVILEGED')
            code_start = copy.copy(
                section_info["ER_IROM_NS_PRIVILEGED"]["start"])
            code_end = copy.copy(
                code_start + section_info["ER_IROM_NS_PRIVILEGED"]["size"])

        # walk symbol section
        # https://python.hotexamples.com/zh/examples/capstone/Cs/-/python-cs-class-examples.html
        symtab = elf.get_section_by_name(".symtab")
        assert isinstance(symtab, SymbolTableSection)

        return code_data, symtab, task_json, section_info, code_start, code_end

    def process_symbols(self):
        IRQ_entry = []
        handler_count = 0
        PendSV = "0xdeadbeef"
        symbol_list = []
        task_entry = []
        for symbol in self.symtab.iter_symbols():
            sym_size = symbol['st_size']
            sym_addr = symbol['st_value'] - 1
            sym_start = sym_addr
            sym_end = sym_addr + sym_size
            # find all handler entries
            if "_Handler" in symbol.name:
                IRQ_addr = hex(sym_start)

                if IRQ_addr in IRQ_entry:
                    continue
                if "PendSV" in symbol.name:
                    PendSV = hex(sym_start)
                    continue
                handler_count += 1
                if sym_start < CODE_NS:
                    IRQ_entry.append(hex(sym_start))

            # find all task entries
            if symbol.name in self.task_json:
                task_entry.append(hex(sym_start))
                task_entry.append(hex(sym_end))
                write_log(self.out_static_analysis,
                          f'find task: {symbol.name}\n', 'a+')

        # capstone cannot disasemble inline asm, so needs to jump those instructions
            if hex(sym_addr) not in symbol_list:
                symbol_list.append(hex(sym_addr))

        # capstone cannot disasemble inline asm, so needs to jump those instructions
        return symbol_list, IRQ_entry, handler_count, task_entry, PendSV

    def get_mask(self, addr):
        if hex(addr) in self.sym_list:
            return bitarray('10000000')
        else:
            return bitarray('00000000')

    def create_ins_tag_4(self, ins_type, ins_size):
        for i in range(0, int(ins_size/2)):
            self.ins_tag += ins_type

    def metadata_analysis_all_4(self):
        icall_count = 0
        icall_list = []
        ibranch_count = 0
        ibranch_list = []
        dbranch_count = 0
        dbranch_list = []
        dcall_count = 0
        dcall_list = []
        loop_count = 0
        ret_count = 0
        ret_list = []
        handler_count = 0
        IRQ_entry = []
        PendSV = "0xdeadbeef"
        task_entry = []
        loop_entry = []
        disassembled = {}
        ins_tag = bitarray()

        ins_type = {'icall': bitarray('0000'), 'ibranch': bitarray('0001'), 'dcall': bitarray('0010'), 'dbranch': bitarray(
            '0011'), 'ret': bitarray('0100'), 'loop': bitarray('0101'), 'other': bitarray('0110'), 'irq': bitarray('0111')}

        capstone_proj = Cs(CS_ARCH_ARM, CS_MODE_ARM +
                           CS_MODE_THUMB + CS_MODE_MCLASS)
        capstone_proj.detail = True

        tmp_start = self.code_start
        tmp_end = self.code_end

        tmp_offset = tmp_start
        tmp_sym_addr = tmp_start

        while tmp_offset <= tmp_end:
            tmp = self.get_data_section(tmp_offset, tmp_end)

            if tmp:
                disassembled = capstone_proj.disasm(tmp, tmp_sym_addr)

            # ------ start to check each instruction in this symbol
            if disassembled:
                pre_instr = ''
                for i, instr in enumerate(disassembled):
                    # branches including: b, beq, bgt, bge, ble, bx, and blx
                    # calls: direct call->bl, icall->blx
                    ''' get the machine code: text '''
                    text = ''
                    for j in range(instr.size):
                        text += '%02x' % instr.bytes[j]
                    ins_size = int(len(text)/2)

                    ''' analyze loops '''
                    if (instr.group(arm.ARM_GRP_JUMP)):
                        for operand in instr.operands:
                            if operand.type == arm.ARM_OP_IMM:
                                if instr.address == operand.imm:
                                    #  use hex() to convert hex integer to hex string
                                    if hex(operand.imm) in loop_entry:
                                        continue
                                    loop_count += 1
                                    loop_entry.append(
                                        hex(operand.imm))
                    # analyze if this is the irq entry
                    if hex(instr.address) in IRQ_entry:
                        self.create_ins_tag_4((ins_type["irq"]), ins_size)
                    # analyze all return instructions
                    # bx lr
                    elif str(text) == str(7047):
                        ret_count = ret_count + 1
                        ret_list.append(hex(instr.address))
                        self.create_ins_tag_4((ins_type["ret"]), ins_size)
                    #   200ae6:	f64f 6c11 	movw	ip, #65041	; 0xfe11
                    #   200aea:	f2c1 0c1f 	movt	ip, #4127	; 0x101f
                    #   200aee:	4760      	bx	ip
                    elif str(text) == str(6047):
                        self.create_ins_tag_4((ins_type["other"]), ins_size)
                    # pop {..., pc} / exclude pop {..., lr}
                    elif instr.id == arm.ARM_INS_POP:
                        ret_flag = 0
                        (regs_read, regs_write) = instr.regs_access()
                        if len(regs_write) > 0:
                            for r in regs_write:
                                if (instr.reg_name(r) == "pc"):
                                    ret_flag = 1
                                    ret_list.append(
                                        hex(instr.address))
                                    self.create_ins_tag_4(
                                        (ins_type["ret"]), ins_size)
                        if ret_flag == 1:
                            ret_count = ret_count + 1
                        else:
                            self.create_ins_tag_4(
                                (ins_type["other"]), ins_size)
                    elif (instr.id == arm.ARM_INS_LDMDA) | (instr.id == arm.ARM_INS_LDMDB) | (instr.id == arm.ARM_INS_LDM) | (instr.id == arm.ARM_INS_LDMIB):
                        ret_flag = 0
                        (regs_read, regs_write) = instr.regs_access()
                        if len(regs_write) > 0:
                            for r in regs_write:
                                if (instr.reg_name(r) == "pc"):
                                    ret_flag = 1
                                    ret_list.append(
                                        hex(instr.address))
                                    self.create_ins_tag_4(
                                        (ins_type["ret"]), ins_size)
                        if ret_flag == 1:
                            ret_count += 1
                        else:
                            self.create_ins_tag_4(
                                (ins_type["other"]), ins_size)
                    # analyze all branch instructions
                    # tbh/tbb
                    elif (instr.id == arm.ARM_INS_TBH):
                        # 204560:	f200 813b 	bhi.w	2047da <processMarkers+0x45e>
                        # 204564:	e8df f011 	tbh	[pc, r1, lsl #1]
                        for pre_operand in pre_instr.operands:
                            if pre_operand.type == arm.ARM_OP_IMM:
                                src = string_int(instr.address)
                                dst = string_int(pre_operand.imm)
                                self.branch_tab_32.append(concat_32(src, dst))
                                ibranch_list.append(concat_32(src, dst))
                                ibranch_count += 1
                                self.create_ins_tag_4(
                                    (ins_type["ibranch"]), ins_size)
                    elif (instr.id == arm.ARM_INS_TBB):
                        # 204560:	f200 813b 	bhi.w	2047da <processMarkers+0x45e>
                        # 204564:	e8df f011 	tbh	[pc, r1, lsl #1]
                        for pre_operand in pre_instr.operands:
                            if pre_operand.type == arm.ARM_OP_IMM:
                                src = string_int(instr.address)
                                dst = string_int(pre_operand.imm)
                                self.branch_tab_32.append(concat_32(src, dst))
                                ibranch_list.append(concat_32(src, dst))
                                ibranch_count += 1
                                self.create_ins_tag_4(
                                    (ins_type["ibranch"]), ins_size)
                    # b/bx/blx/bl/call Rx. Check if the blx Rx are tail calls
                    elif (instr.id == arm.ARM_INS_B) | (instr.id == arm.ARM_INS_CBZ) | (instr.id == arm.ARM_INS_CBNZ):
                        dbranch_count += 1
                        dbranch_list.append(hex(instr.address))
                        self.create_ins_tag_4((ins_type["dbranch"]), ins_size)
                    elif (instr.id == arm.ARM_INS_BX):
                        src = string_int(instr.address)
                        dst = 0
                        ibranch_count += 1
                        self.create_ins_tag_4((ins_type["ibranch"]), ins_size)
                    elif (instr.id == arm.ARM_INS_BLX):
                        icall_count += 1
                        icall_list.append(hex(instr.address))
                        self.create_ins_tag_4((ins_type["icall"]), ins_size)
                    # bl 0xxxx
                    elif (instr.id == arm.ARM_INS_BL):
                        dcall_count += 1
                        dcall_list.append(hex(instr.address))
                        self.create_ins_tag_4((ins_type["dcall"]), ins_size)
                    else:
                        self.create_ins_tag_4((ins_type["other"]), ins_size)
                    tmp = instr
                    tmp_sym_addr = instr.address + ins_size
                    pre_instr = instr

            # ignore 4 bytes instruction
            tmp_sym_addr = tmp_sym_addr + 4
            tmp_offset = tmp_sym_addr
            self.create_ins_tag_4((ins_type["other"]), 4)

        return icall_count, icall_list, ibranch_count, ibranch_list, dcall_count, dcall_list, ret_count, ret_list, dbranch_list, loop_count, loop_entry

    def get_ibt(self):
        branch_tab_32 = []
        output_ibt_json = self.out_metadata_path + self.filename + ".json"
        edge_table = analyze_on_target(
            self.filepath, self.out_metadata_path).edge_table
        analyze_on_target(self.filepath, self.out_metadata_path).write_json(
            output_ibt_json)
        for i in edge_table:
            src = int(edge_table[i]["src_addr"], 16)
            dst = int(edge_table[i]["dst_addr"], 16)
            # filter out UnresolvableCallTarget, we do not consider the unresulvable call target as an ibt entry
            if (dst != 0x30004f) and (dst != 0x30004b):
                record = hex((src & 0x0000ffff) << 16 | (dst & 0x0000ffff))
                if record not in branch_tab_32:
                    branch_tab_32.append(record)

        return branch_tab_32

    def sort_string(self, list):
        if list:
            list_int = sorted([int(x, 0) for x in list])
            return list_int
        return ""

    def format_record(self, list):
        if list:
            return ', '.join(hex(x) for x in list)
        return ""

    def sort_records(self):
        sorted_branch = self.sort_string(self.branch_tab_32)
        sorted_IRQ_entry = self.sort_string(self.IRQ_entry)
        sorted_task_entry = self.sort_string(self.task_entry)
        sorted_loop_entry = self.sort_string(self.loop_entry)

        self.sorted_branch_hex = self.format_record(
            self.sort_string(self.branch_tab_32))
        self.sorted_IRQ_entry_hex = self.format_record(
            self.sort_string(self.IRQ_entry))
        self.sorted_task_entry_hex = self.format_record(
            self.sort_string(self.task_entry))
        self.sorted_loop_entry_hex = self.format_record(
            self.sort_string(self.loop_entry))

        final_icall_list = self.format_record(
            self.sort_string(self.icall_list))
        final_ibranch_list = self.format_record(
            self.sort_string(self.ibranch_list))
        final_dcall_list = self.format_record(
            self.sort_string(self.dcall_list))
        final_ret_list = self.format_record(self.sort_string(self.ret_list))
        final_dbranch_list = self.format_record(
            self.sort_string(self.dbranch_list))

        return sorted_branch, sorted_IRQ_entry, sorted_task_entry, sorted_loop_entry, final_icall_list, final_ibranch_list, final_dcall_list, final_ret_list, final_dbranch_list

    def write_tmp(self):
        # check if it is a file
        if os.path.isfile(self.filepath):
            output_header = self.out_metadata_path + self.filename + "_metadata.h"
            output_dump = self.out_metadata_path + self.filename + ".dump"
            command = "arm-none-eabi-objdump.exe -d " + self.filepath + " > " + output_dump
            os.system(command)

            with open(output_header, "w") as metadata_header:
                metadata_header.write(f'''
#ifndef BT_S_H
#define BT_S_H

/********************************************
* used for run-time control flow validation *
*********************************************/

#include <stdint.h>

#define NUM_BRANCH_PAIRS {int(len(self.branch_tab_32))}
#define NUM_HANDLER {int(len(self.IRQ_entry))}
#define NUM_TASK {int(len(self.task_entry)/2)}
#define NUM_LOOP {int(len(self.loop_entry))}

const uint32_t icall_count = {self.icall_count}; 
const uint32_t ibranch_count = {len(self.branch_tab_32)}; 
const uint32_t dbranch_count = {len(self.dbranch_list)};
const uint32_t dcall_count = {self.dcall_count}; 
const uint32_t ret_count = {self.ret_count}; 
const uint32_t handler_count = {self.handler_count}; 
const uint32_t PendSV = {self.PendSV};

const uint32_t PendSV_Entry = {self.PendSV};

/* branch_tab_64: <src, dst> */
const uint32_t branch_tab_32[] = {{ {self.sorted_branch_hex} }};

/* exception_tab: <start_addr, end_addr> */
const uint32_t IRQ[] = {{ {self.sorted_IRQ_entry_hex} }};
const uint32_t TASK[] = {{ {self.sorted_task_entry_hex} }};
const uint32_t LOOP[] = {{ {self.sorted_loop_entry_hex} }};

const uint32_t icall_list[] = {{ {self.final_icall_list} }};
const uint32_t ibranch_list[] = {{ {self.final_ibranch_list} }};
const uint32_t dcall_list[] = {{ {self.final_dcall_list} }};
const uint32_t ret_list[] = {{ {self.final_ret_list} }};
const uint32_t dbranch_list[] = {{ {self.final_dbranch_list} }};


#endif // BT_S
                ''')
                metadata_header.close()

    # generate metadata bin file
    """
    typedef struct __attribute__((__packed__))
    {
        uint32_t dwMagicCode; // header, __MGAIC_CODE__
        int32_t IBT_size;
        int32_t IRQ_size;
        int32_t TASK_size;
        int32_t LOOP_size;
        int32_t INS_tag_size;
        uint32_t PendSV;
        uint32_t *IBT_entry;
        uint32_t *IRQ_entry;
        uint32_t *TASK_entry;
        uint32_t *LOOP_entry;
    #if defined(INS_4)
        ins_tag_type *INS_tag;
    #elif defined(INS_8)
        uint8_t *INS_tag;
    #endif
    } BtHeader;
    """

    def write_metadata(self, tag_size):
        dwMagicCode = struct.pack('=I', 0xdeadbeef)
        bt_size = struct.pack('=I', len(self.branch_tab_32))
        IRQ_size = struct.pack('=I', len(self.IRQ_entry))
        TASK_size = struct.pack('=I', int(len(self.task_entry)/2))
        LOOP_size = struct.pack('=I', int(len(self.loop_entry)))
        # we do not need the tags, so disable it
        if INS_TAGGING:
            INS_tag_size = struct.pack('=I', int(len(self.ins_tag)/8))
        else:
            INS_tag_size = struct.pack('=I', 0)

        PendSV = struct.pack('=I', int(self.PendSV, 16))
        Branch_pointer = struct.pack('=I', 0)
        IRQ_entry_pointer = struct.pack('=I', 0)
        TASK_entry_pointer = struct.pack('=I', 0)
        LOOP_entry_pointer = struct.pack('=I', 0)
        INS_tag_pointer = struct.pack('=I', 0)

        branch_byte = bytearray()
        IRQ_byte = bytearray()
        task_byte = bytearray()
        loop_byte = bytearray()

        for i in range(len(self.sorted_branch)):
            branch_byte = branch_byte + \
                struct.pack("=I", self.sorted_branch[i])

        for i in range(len(self.sorted_IRQ_entry)):
            IRQ_byte = IRQ_byte + \
                struct.pack("=I", self.sorted_IRQ_entry[i])

        for i in range(len(self.sorted_task_entry)):
            task_byte = task_byte + \
                struct.pack("=I", self.sorted_task_entry[i])

        for i in range(len(self.sorted_loop_entry)):
            loop_byte = loop_byte + \
                struct.pack("=I", self.sorted_loop_entry[i])

        output_bin = self.out_metadata_path + self.filename + ".bin"
        with open(output_bin, "wb") as result:
            result.write(dwMagicCode)
            result.write(bt_size)
            result.write(IRQ_size)
            result.write(TASK_size)
            result.write(LOOP_size)
            result.write(INS_tag_size)

            result.write(PendSV)
            result.write(Branch_pointer)
            result.write(IRQ_entry_pointer)
            result.write(TASK_entry_pointer)
            result.write(LOOP_entry_pointer)
            result.write(INS_tag_pointer)

            if self.sorted_branch:
                result.write(branch_byte)
            if self.sorted_IRQ_entry:
                result.write(IRQ_byte)
            if self.sorted_task_entry:
                result.write(task_byte)
            if self.sorted_loop_entry:
                result.write(loop_byte)
            if INS_TAGGING:
                if self.ins_tag:
                    result.write(self.ins_tag)
                result.close()

    def write_eval_result(self):
        out_result = self.out_log_path + 'eval_result.csv'
        if os.path.exists(out_result):
            df = pd.read_csv(out_result)
            row_to_add = pd.DataFrame({'name': [self.filename], 'icall': [self.icall_count], 'ibranch': [len(self.branch_tab_32)], 'dbranch': [
                                      len(self.dbranch_list)], 'dcall': [self.dcall_count], 'ret': [self.ret_count], 'irq': [self.handler_count]})
            df = df.append(row_to_add)
            df.to_csv(out_result, index=False)
        else:
            write_log(self.out_static_analysis,
                      "missing eval_result file", 'a+')
            return

    def create_metadata_by_name_run(self):
        self.write_tmp()
        self.write_metadata('4')
        self.write_eval_result()


class EvaluationByName(object):
    def __init__(self, proj_name, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.mps_path = "E:/SOFTWARE/"
        self.mps_eval_log = self.mps_path + 'eval.log'
        self.mps_eval_record = self.mps_path + 'record.log'
        self.proj_name = proj_name
        self.sherloc_path = out_binary_s_path
        self.proj_elf = out_binary_ns_path + proj_name + '.axf'
        self.proj_metadata = out_metadata_path + proj_name + '.bin'
        self.proj_eval_name = proj_name + '_' + eval_level
        self.proj_eval_log_name = self.proj_eval_name + '.log'
        self.proj_eval_log = out_log_path + self.proj_eval_log_name
        self.out_eval_log = out_log_path + 'eval.log'
        self.out_eval_record = out_log_path + 'record.log'
        self.eval_record_list = []
        self.eval_level = eval_level

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def load_record(self):
        if os.path.exists(self.out_eval_record):
            with open(self.out_eval_record, 'r') as eval_records:
                self.eval_record_list = [line.rstrip()
                                         for line in eval_records]
        else:
            with open(self.out_eval_record, 'w') as fp:
                pass

    # ret: 0-->empty result; 1-->baseline; 2-->baseline, ss; 3-->
    def check_record(self):
        if self.proj_eval_name in self.eval_record_list:
            print(self.proj_eval_name + " has already done.")
            return 1
        return 0

    def load_image_ns(self):
        mps_image = self.mps_path + 'ns.axf'
        mps_metadata = self.mps_path + 'bs.bin'
        BoardOperation('', self.out_eval_log, self.proj_elf, mps_image,
                       self.hw_reset_port, self.sw_reset_port).load_file()
        BoardOperation('', self.out_eval_log, self.proj_metadata,
                       mps_metadata, self.hw_reset_port, self.sw_reset_port).load_file()

    def eval_run_instance(self, log_file, eval_record_name):
        BoardOperation('sw', self.out_eval_log, '', '',
                       self.hw_reset_port, self.sw_reset_port).run_board_operation()
        print("start running code")
        serialPort = serial.Serial(self.uart_port, 115200)

        tic = time.time()

        buffer = bytearray()
        read_byte = serialPort.read(1)

        IBT_list = []

        while (time.time() - tic < 100000):
            # Read data out of the buffer until a return / new line is found
            if read_byte == b')':
                # read the '/n' and drop
                serialPort.read(2)
                print(
                    "Please wait for seconds for evaluation results collection (check eval.log to see details)...")
                # clear read_byte
                buffer = bytearray()
                read_byte = b''
            elif read_byte == b'#':
                print("emulation mode, collecting IBT")
                # receive: (#:0x456447da\r\n)
                read_byte = serialPort.read(1)  # filter '#'
                write_log(self.out_eval_log, read_byte, 'ab+')
                emu_addr = bytearray()
                read_byte = serialPort.read(1)
                write_log(self.out_eval_log, read_byte, 'ab+')
                while read_byte != b'\r':
                    emu_addr += read_byte
                    read_byte = serialPort.read(1)
                    write_log(self.out_eval_log, read_byte, 'ab+')
                print(emu_addr)
                print("insert new record to IBT")
                # write log to eval_log
                write_log(
                    self.out_eval_log, f' {eval_record_name} insert new record {emu_addr} to IBT.\n', 'a+')
                MetaDataRefresh(self.out_eval_log,
                                self.proj_metadata, emu_addr).refresh_run()
                write_log(self.out_eval_log, eval_record_name +
                          " insert success.\n", 'a+')
                BoardOperation('hw', self.out_eval_log, '', '',
                               self.hw_reset_port, self.sw_reset_port).run_board_operation()
                return "emu"
            elif read_byte == b'U':
                print("target application stuck, hardware reset the board.")
                write_log(self.out_eval_log,
                          eval_record_name + " stuck.\n", 'a+')
                BoardOperation('hw', self.out_eval_log, '', '',
                               self.hw_reset_port, self.sw_reset_port).run_board_operation()
                break
            elif (read_byte == b'F') or (read_byte == b'&'):
                print("read done")
                # write log to eval_log
                write_log(self.out_eval_log, '\n' +
                          eval_record_name + " testing success.\n", 'a+')
                # write evaluated proj in record.log
                write_log(self.out_eval_record,
                          self.proj_eval_name + '\n', 'a+')
                break

            buffer += read_byte
            write_log(self.out_eval_log, read_byte, 'ab+')
            if (len(buffer) > 16*1000):
                try:
                    write_log(log_file, buffer, 'ab+')
                except:
                    print(f'{log_file} does not exist!')
                    pass
                buffer = bytearray()

            read_byte = serialPort.read(1)

        serialPort.close()

        try:
            write_log(log_file, buffer, 'ab+')
        except:
            print(f'{log_file} does not exist!')
            pass

        print('write evaluation log done. hardware reset the board\n')
        BoardOperation('hw', self.out_eval_log, '', '',
                       self.hw_reset_port, self.sw_reset_port).run_board_operation()

    def eval_by_name_run(self):
        if os.path.exists(self.mps_path):
            # check which bench has been evaluated
            self.load_record()
            if self.check_record() == 0:
                print(f'Run {self.proj_eval_name}')
                self.load_image_ns()
                return self.eval_run_instance(self.proj_eval_log, self.proj_eval_log_name)

        else:
            print("hardware reset the system and keep running.\n")
            BoardOperation('hw', self.out_eval_log, '', '',
                           self.hw_reset_port, self.sw_reset_port).run_board_operation()
            time.sleep(5)
            if os.path.exists(self.mps_path):
                print("Please check if the board has mounted its file system. \n")
                sys.exit()
            return

# other can be blinky systick frequency or freertos task numbers


class EmulationForTarget(object):
    def __init__(self, proj_name, other, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.proj_name = f'{proj_name}_{other}'
        self.other = other
        self.sherloc_path = out_binary_s_path
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.eval_level = eval_level
        self.out_eval_log = out_log_path + 'eval.log'

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def load_SHERLOC(self):
        mps_image = 'E:/SOFTWARE/s.axf'
        SHERLOC_image = self.sherloc_path + self.eval_level + '.axf'
        BoardOperation('', self.out_eval_log,
                       SHERLOC_image, mps_image, self.hw_reset_port, self.sw_reset_port).load_file()

    def emu_run(self):
        self.load_SHERLOC()
        start = time.time()
        ret = EvaluationByName(self.proj_name, self.sherloc_path, self.out_binary_ns_path,
                               self.out_metadata_path, self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        print(ret)
        while ret == "emu":
            print("continue emulation")
            ret = EvaluationByName(self.proj_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path,
                                   self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        EvaluationByName(self.proj_name, self.sherloc_path, self.out_binary_ns_path,
                         self.out_metadata_path, self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        print("time elapsed: ", time.time() - start)


class EvaluationForTarget(object):
    def __init__(self, proj_name, other, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.proj_name = f'{proj_name}_{other}'
        self.sherloc_path = out_binary_s_path
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.eval_level = eval_level
        self.out_eval_log = out_log_path + 'eval.log'

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def load_SHERLOC(self):
        mps_image = 'E:/SOFTWARE/s.axf'
        SHERLOC_image = self.sherloc_path + self.eval_level + '.axf'
        BoardOperation('', self.out_eval_log,
                       SHERLOC_image, mps_image, self.hw_reset_port, self.sw_reset_port).load_file()

    def eval_run(self):
        self.load_SHERLOC()
        start = time.time()
        EvaluationByName(self.proj_name, self.sherloc_path, self.out_binary_ns_path,
                         self.out_metadata_path, self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        print("time elapsed: ", time.time() - start)
