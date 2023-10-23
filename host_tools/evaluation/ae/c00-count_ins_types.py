#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311
"""
description: static analysis the target systems and output the number of different control-flow transfers.
usage: python c00-count_ins_types.py > c00-count_ins_types.log
"""

import os
import xml.etree.ElementTree as ET
import copy
import numpy as np

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from capstone import Cs, CS_ARCH_ARM, CS_MODE_THUMB, arm
from capstone import *
import json
from pathlib import Path
from bitarray import bitarray

import sys
sys.path.append('../')
from ibt_analysis import *

INS_TAGGING = 0
TEST_ALL = 1
base = 0x200000
CODE_NS = 0x400000

target_opt_levels = ['O3', 'Oz']
tmp_start = 0x00200400
# for blinky Oz
# tmp_end = 0x00200f5c
# for blinky O3
# tmp_end = 0x00206f1e
eval_elf_s = '../elf_s/'
example_path = '../../../Example/'
eval_working_path = example_path + 'out/eval/'
out_binary_s_path = eval_working_path + 'elf_s/'
out_binary_ns_path = eval_working_path + 'elf_ns/'

class StaticAnalyzerBeebs(object):
    def __init__(self, filepath, out_metadata_path, out_log_path, opt_level, tmp_end_list):
        self.filename = Path(filepath).stem
        self.filepath = filepath
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.out_static_analysis = out_log_path + 'static_analysis.log'
        self.opt_level = opt_level
        self.tmp_end_list = tmp_end_list
        self.base = 0x200000

    def process_elf_for_beebs(self):
        code_data = {}
        section_info = {}
        section_tmp = {}
        # List of control-flow transfer instructions for Armv8-M
        dc = ["bl"]
        dc_count = 0
        db = ["b", "bgt", "beq", "cbnz", "cbz", "tbb", "tbh"]
        db_count  = 0
        ic = ["blx"]
        ic_count = 0
        ib = ["bx"]
        ib_count = 0
        ret_count = 0
        # Symbols to exclude
        exclude_symbols = ["Reset_Handler", "HardFault_Handler", "Default_Handler", "__main", "__scatterload", "__scatterload_rt2", "__scatterload_rt2_thumb_only", "__scatterload_null", "__scatterload_zeroinit", "__rt_lib_init", "__rt_lib_init_fp_1", "__rt_lib_init_alloca_1", "__rt_lib_init_argv_1", "__rt_lib_init_atexit_1", "__rt_lib_init_clock_1", "__rt_lib_init_cpp_1", "__rt_lib_init_exceptions_1", "__rt_lib_init_fp_trap_1", "__rt_lib_init_getenv_1", "__rt_lib_init_heap_1", "__rt_lib_init_lc_collate_1", "__rt_lib_init_lc_ctype_1", "__rt_lib_init_lc_monetary_1", "__rt_lib_init_lc_numeric_1", "__rt_lib_init_lc_time_1", "__rt_lib_init_preinit_1", "__rt_lib_init_rand_1", "__rt_lib_init_relocate_pie_1", "__rt_lib_init_return", "__rt_lib_init_signal_1", "__rt_lib_init_stdio_1", "__rt_lib_init_user_alloc_1", "__rt_lib_shutdown", "__rt_lib_shutdown_cpp_1", "__rt_lib_shutdown_fp_trap_1", "__rt_lib_shutdown_heap_1", "__rt_lib_shutdown_return", "__rt_lib_shutdown_signal_1", "__rt_lib_shutdown_stdio_1", "__rt_lib_shutdown_user_alloc_1", "__rt_entry", "__rt_entry_presh_1", "__rt_entry_sh", "__rt_entry_li", "__rt_entry_postsh_1", "__rt_entry_main", "__rt_entry_postli_1", "__rt_exit", "__rt_exit_ls", "__rt_exit_prels_1", "__rt_exit_exit", "exit", "__I$use$semihosting", "__use_no_semihosting_swi", "__semihosting_library_function", "_sys_exit", "", "$Ven$TT$L$$mtb_enable", "$Ven$TT$L$$time_measurement_s", "_fp_init", "__fplib_config_fpu_vfp", "__fplib_config_pureend_doubles", "Secure_printf", "Secure_printf_int", "mtb_enable", "time_measurement_s", "__scatterload_copy"]  # Modify this list as required

        # open file
        try:
            target_file = open(self.filepath, 'rb')
        except IOError as e:
            print(e)
        elffile = ELFFile(target_file)

        # Load the binary
        # print(f'Processing file: {self.filepath}')

        # read sections
        # print("\n-------------- Section scatter --------------")
        # for section in elffile.iter_sections():
        #     section_tmp["start"] = section['sh_addr']
        #     section_tmp["size"] = section['sh_size']
        #     section_info[section.name] = copy.copy(section_tmp)
        # print(section_info)

        code_section = elffile.get_section_by_name('ER_ROM')
        # Check if .text section exists
        if not code_section:
            print("Error: .text section not found in the ELF file.")
            exit()

        code = code_section.data()

        # Create a Capstone instance for Armv8-M architecture
        # cs = capstone.Cs(CS_ARCH_ARM64, CS_MODE_ARM)
        cs = Cs(CS_ARCH_ARM, CS_MODE_ARM + CS_MODE_THUMB + CS_MODE_MCLASS)
        cs.detail = True
        count = 0

        # Fetch symbols and disassemble based on them
        for symbol in elffile.get_section_by_name('.symtab').iter_symbols():
            if symbol.entry.st_info.type == 'STT_FUNC' and symbol.name not in exclude_symbols:
                start = symbol['st_value'] - 1 - base
                end = start + symbol['st_size']
                # print(f"Symbol: {symbol.name} at start: {hex(start)}, end: {hex(end)}")

                disassembled = cs.disasm(code[start:end], start)
                if disassembled:
                    for i, instr in enumerate(disassembled):

                        ''' get the machine code: text '''
                        text = ''
                        for j in range(instr.size):
                            text += '%02x' % instr.bytes[j]
                        ins_size = int(len(text)/2)

                        # ------------- ret --------------
                        # bx lr
                        if str(text) == str(7047):
                            ret_count += 1
                        # pop {..., pc} / exclude pop {..., lr}
                        elif instr.id == arm.ARM_INS_POP:
                            (regs_read, regs_write) = instr.regs_access()
                            if len(regs_write) > 0:
                                for r in regs_write:
                                    if (instr.reg_name(r) == "pc"):
                                        ret_count += 1
                        elif (instr.id == arm.ARM_INS_LDMDA) | (instr.id == arm.ARM_INS_LDMDB) | (instr.id == arm.ARM_INS_LDM) | (instr.id == arm.ARM_INS_LDMIB):
                            (regs_read, regs_write) = instr.regs_access()
                            if len(regs_write) > 0:
                                for r in regs_write:
                                    if (instr.reg_name(r) == "pc"):
                                        ret_count += 1

                        # ------------- branch --------------
                        # tbh/tbb
                        elif (instr.id == arm.ARM_INS_TBH):
                            db_count += 1
                        elif (instr.id == arm.ARM_INS_TBB):
                            db_count += 1
                        # b/bx/blx/bl/call Rx. Check if the blx Rx are tail calls
                        elif (instr.id == arm.ARM_INS_B) | (instr.id == arm.ARM_INS_CBZ) | (instr.id == arm.ARM_INS_CBNZ):
                            db_count += 1
                        elif (instr.id == arm.ARM_INS_BX):
                            ib_count += 1
                        elif (instr.id == arm.ARM_INS_BLX):
                            ic_count += 1
                        # bl 0xxxx
                        elif (instr.id == arm.ARM_INS_BL):
                            # print out the oprands and instructions
                            # print(f"{text}: {instr}")
                            # check if the target address is in this symbol range
                            if (instr.operands[0].type == arm.ARM_OP_IMM) and (instr.operands[0].value.imm >= start) and (instr.operands[0].value.imm <= end + 18):
                                continue
                            # print(f"in this symbol {text}: {instr}")
                            dc_count += 1                        

        print(f"{self.filename}ic: {ic_count}, ib: {ib_count}, dc: {dc_count}, db: {db_count}, ret: {ret_count}") 

class StaticAnalyzer(object):
    def __init__(self, filepath, out_metadata_path, out_log_path, opt_level, tmp_end_list):
        self.filename = Path(filepath).stem
        self.filepath = filepath
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.out_static_analysis = out_log_path + 'static_analysis.log'
        self.opt_level = opt_level
        self.tmp_end_list = tmp_end_list
        self.base = 0x200000
        self.ins_tag = bitarray()
        self.branch_tab_32 = []

        self.code_data, self.symtab, self.task_json, self.section_info, self.code_start, self.code_end = self.process_elf()

        self.sym_list, self.IRQ_entry, self.handler_count, self.task_entry, self.PendSV = self.process_symbols()

        self.icall_count, self.icall_list, self.ibranch_count, self.ibranch_list, self.dcall_count, self.dcall_list, self.ret_count, self.ret_list, self.dbranch_list, self.loop_count, self.loop_entry = self.metadata_analysis_all()

    """
    code_data_tmp["start"] = code_ns_unprivileged['sh_addr']
    code_data_tmp["end"] = code_ns_unprivileged['sh_addr'] + \
        code_ns_unprivileged['sh_size']
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

        # print(f'Processing file: {self.filepath}')

        code_data = {}
        section_info = {}
        section_tmp = {}
        code_start = 0
        code_end = 0
        # read sections
        print("\n-------------- Section scatter --------------")
        for section in elf.iter_sections():
            section_tmp["start"] = section['sh_addr']
            section_tmp["size"] = section['sh_size']
            section_info[section.name] = copy.copy(section_tmp)

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

        # # read the instruction from .text section
        print("\n-------------- Instructions <ins_addr : mc : ops> --------------")

        # walk symbol section
        # https://python.hotexamples.com/zh/examples/capstone/Cs/-/python-cs-class-examples.html
        symtab = elf.get_section_by_name(".symtab")
        assert isinstance(symtab, SymbolTableSection)

        return code_data, symtab, task_json, section_info, code_start, code_end

    def process_symbols(self):
        IRQ_entry = []
        handler_count = 0
        PendSV = "0xdeadbeef"
        symbol_list = {}
        symbol_name = []
        symbol_addr = []
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
                print(f'find task: {symbol.name}\n')

        # capstone cannot disasemble inline asm, so needs to jump those instructions
            if hex(sym_addr) not in symbol_addr:
                symbol_name.append(symbol.name)
                symbol_addr.append(hex(sym_addr))
        # capstone cannot disasemble inline asm, so needs to jump those instructions
        symbol_list = dict(zip(symbol_name, symbol_addr))
        print(symbol_list)
        return symbol_addr, IRQ_entry, handler_count, task_entry, PendSV

    def metadata_analysis_all(self):
        icall_count = 0
        icall_list = []
        ibranch_count = 0
        ibranch_list = []
        dbranch_list = []
        dcall_count = 0
        dcall_list = []
        loop_count = 0
        ret_count = 0
        ret_list = []
        loop_entry = []
        disassembled = {}
        sequential_num_list = []
        sequential_num = 0
        sequential_flag = 1

        capstone_proj = Cs(CS_ARCH_ARM, CS_MODE_ARM +
                           CS_MODE_THUMB + CS_MODE_MCLASS)
        capstone_proj.detail = True

        tmp_end = self.tmp_end_list[self.opt_level]

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
                    sequential_flag = 1

                    ''' analyze loops '''
                    # analyze all return instructions
                    # bx lr
                    if str(text) == str(7047):
                        sequential_flag = 0
                    #   200ae6:	f64f 6c11 	movw	ip, #65041	; 0xfe11
                    #   200aea:	f2c1 0c1f 	movt	ip, #4127	; 0x101f
                    #   200aee:	4760      	bx	ip
                    elif str(text) == str(6047):
                        sequential_flag = 0
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
                        if ret_flag == 1:
                            sequential_flag = 0
                    elif (instr.id == arm.ARM_INS_LDMDA) | (instr.id == arm.ARM_INS_LDMDB) | (instr.id == arm.ARM_INS_LDM) | (instr.id == arm.ARM_INS_LDMIB):
                        ret_flag = 0
                        (regs_read, regs_write) = instr.regs_access()
                        if len(regs_write) > 0:
                            for r in regs_write:
                                if (instr.reg_name(r) == "pc"):
                                    ret_flag = 1
                                    ret_list.append(
                                        hex(instr.address))
                        if ret_flag == 1:
                            sequential_flag = 0
                    # analyze all branch instructions
                    # tbh/tbb
                    elif (instr.id == arm.ARM_INS_TBH):
                        sequential_flag = 0
                    elif (instr.id == arm.ARM_INS_TBB):
                        sequential_flag = 0
                    # b/bx/blx/bl/call Rx. Check if the blx Rx are tail calls
                    elif (instr.id == arm.ARM_INS_B) | (instr.id == arm.ARM_INS_CBZ) | (instr.id == arm.ARM_INS_CBNZ):
                        sequential_flag = 0
                    elif (instr.id == arm.ARM_INS_BX):
                        sequential_flag = 0
                    elif (instr.id == arm.ARM_INS_BLX):
                        sequential_flag = 0
                    # bl 0xxxx
                    elif (instr.id == arm.ARM_INS_BL):
                        sequential_flag = 0

                    tmp = instr
                    tmp_sym_addr = instr.address + ins_size

                    if sequential_flag == 1:
                        sequential_num = sequential_num + 1
                    else:
                        sequential_num_list.append(sequential_num)
                        sequential_num = 0

            # ignore 4 bytes instruction
            tmp_sym_addr = tmp_sym_addr + 4
            sequential_num += 1
            tmp_offset = tmp_sym_addr

        print(f'sequential_num: {sequential_num_list}')
        # calculate the min and max sequential number from the sequential_num_list
        sequential_num_min = min(sequential_num_list)
        sequential_num_max = max(sequential_num_list)
        # calculate the mean and median sequential number from the sequential_num_list
        sequential_num_mean = np.mean(sequential_num_list)
        sequential_num_median = np.median(sequential_num_list)
        print(f'sequential_num_min: {sequential_num_min}')
        print(f'sequential_num_max: {sequential_num_max}')
        print(f'sequential_num_median: {sequential_num_median}')
        print(f'sequential_num_mean: {sequential_num_mean}')

        return icall_count, icall_list, ibranch_count, ibranch_list, dcall_count, dcall_list, ret_count, ret_list, dbranch_list, loop_count, loop_entry       

    def analyze_metadata_by_name_run(self):
        print("test")

class StaticAnalyzerForBeebs(object):
    def __init__(self, BEEBS_name_list, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        # self.proj_name = f'{proj_name}'
        self.BEEBS_name_list = BEEBS_name_list
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.opt_level = opt

    def create_metadata_run(self):
        for bench_name in self.BEEBS_name_list:
            # print("\n---------------------\analyze instructions for ", bench_name)
            intput = bench_name + '.axf'
            filepath = os.path.join(self.out_binary_ns_path, intput)
            blinky_tmp_end_list = {'Oz': 0x00200f5c, 'O3': 0x00206f1e}

            object = StaticAnalyzerBeebs(filepath, self.out_metadata_path,
                                    self.out_log_path, self.opt_level, blinky_tmp_end_list)
            object.process_elf_for_beebs()

class StaticAnalyzerForBlinky(object):
    def __init__(self, proj_name, frequency, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        self.proj_name = f'{proj_name}_{frequency}'
        self.frequency = frequency
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.opt_level = opt

    def create_metadata_run(self):
        # print("\n---------------------\analyze instructions percentage for ", self.proj_name)
        intput = self.proj_name + '.axf'
        filepath = os.path.join(self.out_binary_ns_path, intput)
        blinky_tmp_end_list = {'Oz': 0x00200f5c, 'O3': 0x00206f1e}

        object = StaticAnalyzer(filepath, self.out_metadata_path,
                                self.out_log_path, self.opt_level, blinky_tmp_end_list)
        object.analyze_metadata_by_name_run()


class StaticAnalyzerForFreeRTOS(object):
    def __init__(self, proj_name, task_number, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        self.proj_name = f'{proj_name}_{task_number}'
        self.task_number = task_number
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path
        self.opt_level = opt

    def create_metadata_run(self):
        print(
            "\n---------------------\analyze instructions percentage for ", self.proj_name)
        intput = self.proj_name + '.axf'
        filepath = os.path.join(self.out_binary_ns_path, intput)
        freertos_tmp_end_list = {'Oz': 0x00201b28, 'O3': 0x00202874}

        object = StaticAnalyzer(filepath, self.out_metadata_path,
                                self.out_log_path, self.opt_level, freertos_tmp_end_list)
        object.analyze_metadata_by_name_run()


if __name__ == '__main__':

    for opt_level in target_opt_levels:
        # parent directory for working path
        eval_working_subpath = eval_working_path + opt_level + '/'

        out_uvprojx_path = eval_working_subpath + 'proj/'
        out_binary_ns_path = eval_working_subpath + 'elf_ns/'
        out_metadata_path = eval_working_subpath + 'metadata/'
        out_tmp_path = eval_working_subpath + 'build_tmp/'
        out_log_path = eval_working_subpath + 'eval_log/'
        out_eval_log_path = out_log_path + 'eval.log'

        BEEBS_name_list = ['bubblesort', 'crc32', 'dijkstra', 'edn', 'fasta', 'frac', 'levenshtein', 'nbody', 'ndes', 'nettle_aes', 'picojpeg',
                                'qrduino', 'rijndael', 'sglib_arraybinsearch', 'sglib_dllist', 'sglib_hashtable', 'sglib_listsort', 'sglib_queue', 'sglib_rbtree', 'st', 'whetstone']
        print(f'-----------------opt_level: {opt_level}-------------------')
        StaticAnalyzerForBeebs(BEEBS_name_list, out_binary_ns_path, out_metadata_path, out_log_path, opt_level).create_metadata_run()
        # StaticAnalyzerForBlinky('Sherloc_Blinky_ns', '10ms', out_binary_ns_path, out_metadata_path, out_log_path, opt_level).create_metadata_run()
        # StaticAnalyzerForFreeRTOS('FreeRTOS_MPU_ns', 1, out_binary_ns_path, out_metadata_path, out_log_path, opt_level).create_metadata_run()
