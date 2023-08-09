#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311

import os
import shutil

from eval_beebs import *
from eval_blinky import *
from eval_freertos import *
from eval_trigger import *
from eval_vulfoo import *

'''
usage: python .\eval_run.py > eval_run.log
input: None
output: evaluation results, including:
    1. binary for each targeted protect system
    2. metadata for each binary
    3. evaluation log
    4. record file to record which benchmark has been evaluated
    5. evaluation logs for each sub-step with different optimization levels and different target programs
'''


target_opt_levels = ['O3']
eval_elf_s = '../../host_tools/evaluation/elf_s/'
example_path = '../../Example/'
eval_working_path = example_path + 'out/eval/'
out_binary_s_path = eval_working_path + 'elf_s/'
uart_port = 'COM15'
hw_reset_port = 'COM16'
sw_reset_port = 'COM6'


class EvaluationRunnerForBEEBs(object):
    def __init__(self, uart_port, hw_reset_port, sw_reset_port):
        self.eval_substeps = ['baseline', 'ibt_baseline_no_systick',
                              'ibt_no_systick', 'full_no_irq', 'sherloc_read', 'ins']

        # "fir", "slre", "huffbench", "cubic", "mergesort", "sqrt", "wikisort", "matmult_int", "stb_perlin", 'nettle_arcfour', 'sglib_arrayheapsort', 'sglib_arrayquicksort', 'sglib_listinsertsort',
        self.BEEBS_name_list = ['bubblesort', 'crc32', 'dijkstra', 'edn', 'fasta', 'frac', 'levenshtein', 'nbody', 'ndes', 'nettle_aes', 'picojpeg',
                                'qrduino', 'rijndael', 'sglib_arraybinsearch', 'sglib_dllist', 'sglib_hashtable', 'sglib_listsort', 'sglib_queue', 'sglib_rbtree', 'st', 'whetstone']

        self.eval_beebs_src_path = '../../Sherloc_runtime/evaluation/src/'
        self.in_beebs_path = example_path + 'Sherloc_S_NS/'
        self.in_beebs_path_ns = self.in_beebs_path + 'Sherloc_ns/'

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def beebs_prepare_run(self):
        start = time.time()
        #  build target program and create metadata [O3, Oz]
        print(f'-- create .uvprojx with {opt_level}')
        # create xml and header file for each beebs suit
        UVPROJXCreatorForBEEBS(
            self.BEEBS_name_list, self.eval_beebs_src_path, out_uvprojx_path, opt_level).creator_run()
        print(f'-- build keil project with -{opt_level}')
        # build the project to get the binary file
        UVPROJXBuilderForBEEBS(self.BEEBS_name_list, self.in_beebs_path_ns,
                               out_uvprojx_path, out_binary_ns_path, out_tmp_path).build_run()
        print(f'-- static analysis to create metadata with {opt_level}')
        # create metadata for all binaries
        MetadataCreatorForBEEBS(self.BEEBS_name_list, out_binary_ns_path,
                                out_metadata_path, out_log_path, opt_level).create_metadata_run()

        print(f'prepare for the evaluation costs {time.time() - start}')

    def beebs_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        print(f'-- emulation on board to collect IBT with {opt_level}')
        EmulationForBEEBS(self.BEEBS_name_list, out_binary_s_path,
                          out_binary_ns_path, out_metadata_path, out_log_path, "emu_no_irq",  self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()
        print(f'emulation costs {time.time() - start}')

    def beebs_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            print(f'-- start {option} evaluation with {opt_level}')
            EvaluationForBEEBS(self.BEEBS_name_list, out_binary_s_path,
                               out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def beebs_eval_run_all(self):
        # self.beebs_prepare_run()
        # self.beebs_emu_run()
        self.beebs_eval_run()
        print('finish beebs evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


class EvaluationRunnerForBlinky(object):
    def __init__(self, opt_level, uart_port, hw_reset_port, sw_reset_port):
        self.opt_level = opt_level
        self.eval_substeps = ['baseline_blinky', 'full_blinky',
                              'sherloc_read_systick', 'ins_systick', 'ibt_baseline_systick', 'ibt_systick']

        self.TargetName = 'Sherloc_Blinky_ns'
        self.in_blinky_path = example_path + 'Sherloc_Blinky_S_NS/'
        self.in_blinky_path_ns = self.in_blinky_path + 'Sherloc_Blinky_ns/'
        self.eval_blinky_src_path = './blinky/'
        self.frequency_list = ['10ms', '100ms']

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def blinky_prepare_run(self):
        start = time.time()
        print(f'-- build keil project with -{self.opt_level}')
        for frequency in self.frequency_list:
            # build the project to get the binary file
            UVPROJXBuilderForBlinky(self.TargetName, frequency, self.in_blinky_path_ns,
                                    self.eval_blinky_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, self.opt_level).build_run()
            print(
                f'-- static analysis to create metadata with {self.opt_level}')
            # create metadata for all binaries
            MetadataCreatorForBlinky(self.TargetName, frequency, out_binary_ns_path,
                                     out_metadata_path, out_log_path, self.opt_level).create_metadata_run()

        print(f'prepare for the evaluation costs {time.time() - start}')

    def blinky_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        for frequency in self.frequency_list:
            print(
                f'-- emulation on board to collect IBT with {self.opt_level}_{frequency}')
            EmulationForBlinky(self.TargetName, frequency, out_binary_s_path, out_binary_ns_path,
                               out_metadata_path, out_log_path, f"emu_blinky", self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()

        print(f'emulation costs {time.time() - start}')

    def blinky_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            for frequency in self.frequency_list:
                print(
                    f'-- start {option} evaluation with {self.opt_level} and {frequency}')
                EvaluationForBlinky(self.TargetName, frequency, out_binary_s_path,
                                    out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def blinky_eval_run_all(self):
        self.blinky_prepare_run()
        self.blinky_emu_run()
        self.blinky_eval_run()
        print('finish blinky evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


class EvaluationRunnerForFreeRTOS(object):
    def __init__(self, opt_level, uart_port, hw_reset_port, sw_reset_port):
        self.opt_level = opt_level
        self.eval_substeps = ['baseline_rtos', 'full_rtos', 'read_rtos', 'ins_rtos', 'ibt_freertos_baseline', 'ibt_freertos',
                              'MPU_trigger', 'MPU_trigger_read', 'MPU_trigger_ins', 'MPU_trigger_IBT', 'MPU_trigger_IBT_baseline']

        self.TargetName = 'FreeRTOS_MPU_ns'
        self.in_FreeRTOS_path = example_path + 'Sherloc_FreeRTOS_MPU_S_NS/'
        self.in_FreeRTOS_path_ns = self.in_FreeRTOS_path + 'FreeRTOS_MPU_ns/'
        self.eval_FreeRTOS_src_path = './freertos/'
        self.task_number = ['1']

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def FreeRTOS_prepare_run(self):
        start = time.time()
        print(f'-- build keil project with -{self.opt_level}')
        for task_number in self.task_number:
            # build the project to get the binary file
            UVPROJXBuilderForFreeRTOS(self.TargetName, task_number, self.in_FreeRTOS_path_ns,
                                      self.eval_FreeRTOS_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, self.opt_level).build_run()
            print(
                f'-- static analysis to create metadata with {self.opt_level}')
            # create metadata for all binaries
            MetadataCreatorForFreeRTOS(self.TargetName, task_number, out_binary_ns_path,
                                       out_metadata_path, out_log_path, self.opt_level).create_metadata_run()
        print(f'prepare for the evaluation costs {time.time() - start}')

    def FreeRTOS_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        for task_number in self.task_number:
            print(
                f'-- emulation on board to collect IBT with {self.opt_level}_{task_number}')
            EmulationForFreeRTOS(self.TargetName, task_number, out_binary_s_path, out_binary_ns_path,
                                 out_metadata_path, out_log_path, f"full_emu", self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()

        print(f'emulation costs {time.time() - start}')

    def FreeRTOS_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            for task_number in self.task_number:
                print(
                    f'-- start {option} evaluation with {self.opt_level} and {task_number}')
                EvaluationForFreeRTOS(self.TargetName, task_number, out_binary_s_path,
                                      out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def FreeRTOS_eval_run_all(self):
        self.FreeRTOS_prepare_run()
        self.FreeRTOS_emu_run()
        self.FreeRTOS_eval_run()
        print('finish FreeRTOS evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


class EvaluationRunnerForLEDTrigger(object):
    def __init__(self, opt_level, uart_port, hw_reset_port, sw_reset_port):
        self.opt_level = opt_level
        self.eval_substeps = ['baseline_rtos', 'led_trigger', 'led_trigger_read',
                              'led_trigger_ins', 'led_trigger_IBT', 'led_trigger_IBT_baseline']

        self.TargetName = 'FreeRTOS_MPU_ns'
        self.in_trigger_path = example_path + 'Trigger_S_NS/'
        self.in_trigger_path_ns = self.in_trigger_path + 'FreeRTOS_MPU_ns/'
        self.eval_trigger_src_path = './led_trigger/'
        self.task_number = ['led']

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def trigger_prepare_run(self):
        start = time.time()
        print(f'-- build keil project with -{self.opt_level}')
        for task_number in self.task_number:
            if task_number == 'mpu':
                task_number
            # build the project to get the binary file
            UVPROJXBuilderForTrigger(self.TargetName, task_number, self.in_trigger_path_ns,
                                     self.eval_trigger_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, self.opt_level).build_run()
            print(
                f'-- static analysis to create metadata with {self.opt_level}')
            # create metadata for all binaries
            MetadataCreatorForTrigger(self.TargetName, task_number, out_binary_ns_path,
                                      out_metadata_path, out_log_path, self.opt_level).create_metadata_run()

        print(f'prepare for the evaluation costs {time.time() - start}')

    def trigger_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        for task_number in self.task_number:
            print(
                f'-- emulation on board to collect IBT with {self.opt_level}_{task_number}')
            EmulationForTarget(self.TargetName, task_number, out_binary_s_path, out_binary_ns_path,
                               out_metadata_path, out_log_path, f"full_emu", self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()

        print(f'emulation costs {time.time() - start}')

    def trigger_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            for task_number in self.task_number:
                print(
                    f'-- start {option} evaluation with {self.opt_level} and {task_number}')
                EvaluationForTarget(self.TargetName, task_number, out_binary_s_path,
                                    out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def trigger_eval_run_all(self):
        # need to careful about whether the bin file has been updated or not.
        # check the create_metadata_by_name_run() function in eval.py file
        self.trigger_prepare_run()
        self.trigger_emu_run()
        self.trigger_eval_run()
        print('finish led trigger evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


class EvaluationRunnerForVulfooBOF(object):
    def __init__(self, opt_level, uart_port, hw_reset_port, sw_reset_port):
        self.opt_level = opt_level
        self.eval_substeps = ['baseline_systick', 'sherloc_read_systick',
                              'ins_systick', 'full_rtos']
        self.eval_substeps = ['full_rtos']
        self.TargetName = 'FreeRTOS_MPU_ns'
        self.in_vulfoo_path = example_path + 'Vulfoo_S_NS/'
        self.in_vulfoo_path_ns = self.in_vulfoo_path + 'FreeRTOS_MPU_ns/'
        self.eval_vulfoo_src_path = './vulfoo/'
        self.task_number = ['novul', 'vul']

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def vulfoo_prepare_run(self):
        start = time.time()
        print(f'-- build keil project with -{self.opt_level}')
        for task_number in self.task_number:
            # build the project to get the binary file
            UVPROJXBuilderForVulfoo(self.TargetName, task_number, self.in_vulfoo_path_ns,
                                    self.eval_vulfoo_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, self.opt_level).build_run()
            print(
                f'-- static analysis to create metadata with {self.opt_level}')
            # create metadata for all binaries
            MetadataCreatorForVulfoo(self.TargetName, task_number, out_binary_ns_path,
                                     out_metadata_path, out_log_path, self.opt_level).create_metadata_run()

        print(f'prepare for the evaluation costs {time.time() - start}')

    def vulfoo_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        task_number = 'novul'
        print(
            f'-- emulation on board to collect IBT with {self.opt_level}_{task_number}')
        EmulationForTarget(self.TargetName, task_number, out_binary_s_path, out_binary_ns_path,
                           out_metadata_path, out_log_path, f"full_emu", self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()

        print(f'emulation costs {time.time() - start}')

    def vulfoo_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            task_number = 'vul'
            print(
                f'-- start {option} evaluation with {self.opt_level} and {task_number}')
            EvaluationForTarget(self.TargetName, task_number, out_binary_s_path,
                                out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def vulfoo_eval_run_all(self):
        # self.vulfoo_prepare_run()
        # self.vulfoo_emu_run()
        self.vulfoo_eval_run()
        print('finish vulfoo evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


class EvaluationRunnerForVulfooTask(object):
    def __init__(self, opt_level, uart_port, hw_reset_port, sw_reset_port):
        self.opt_level = opt_level
        # self.eval_substeps = ['baseline_systick', 'sherloc_read_systick',
        #                       'ins_systick', 'full_rtos']
        self.eval_substeps = ['full_rtos']
        self.TargetName = 'FreeRTOS_MPU_ns'
        self.in_vulfoo_path = example_path + 'Vulfoo_Task_S_NS/'
        self.in_vulfoo_path_ns = self.in_vulfoo_path + 'FreeRTOS_MPU_ns/'
        self.eval_vulfoo_src_path = './vulfoo_task/'
        self.task_number = ['task_novul', 'task_vul']

        self.uart_port = uart_port
        self.hw_reset_port = hw_reset_port
        self.sw_reset_port = sw_reset_port

    def task_vulfoo_prepare_run(self):
        start = time.time()
        print(f'-- build keil project with -{self.opt_level}')
        for task_number in self.task_number:
            # build the project to get the binary file
            UVPROJXBuilderForVulfoo(self.TargetName, task_number, self.in_vulfoo_path_ns,
                                    self.eval_vulfoo_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, self.opt_level).build_run()
            print(
                f'-- static analysis to create metadata with {self.opt_level}')
            # create metadata for all binaries
            MetadataCreatorForVulfoo(self.TargetName, task_number, out_binary_ns_path,
                                     out_metadata_path, out_log_path, self.opt_level).create_metadata_run()

        print(f'prepare for the evaluation costs {time.time() - start}')

    def task_vulfoo_emu_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # emulated run to update the metadata
        task_number = 'task_novul'
        print(
            f'-- emulation on board to collect IBT with {self.opt_level}_{task_number}')
        EmulationForTarget(self.TargetName, task_number, out_binary_s_path, out_binary_ns_path,
                           out_metadata_path, out_log_path, f"full_rtos", self.uart_port, self.hw_reset_port, self.sw_reset_port).emu_run()

        print(f'emulation costs {time.time() - start}')

    def task_vulfoo_eval_run(self):
        start = time.time()
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()
        # start to get the evaluation result
        for option in self.eval_substeps:
            task_number = 'task_vul'
            print(
                f'-- start {option} evaluation with {self.opt_level} and {task_number}')
            EvaluationForTarget(self.TargetName, task_number, out_binary_s_path,
                                out_binary_ns_path, out_metadata_path, out_log_path, option,  self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_run()

        print(f'evaluate sub-steps costs {time.time() - start}')

    def task_vulfoo_eval_run_all(self):
        # self.task_vulfoo_prepare_run()
        # self.task_vulfoo_emu_run()
        self.task_vulfoo_eval_run()
        print('finish vulfoo evaluation, hardware reset the board...')
        BoardOperation('hw', out_eval_log, '', '', self.hw_reset_port,
                       self.sw_reset_port).run_board_operation()


if __name__ == '__main__':
    # clean output
    if os.path.exists(eval_working_path):
        print("find eval output directory")
        # shutil.rmtree(eval_working_path)
    else:
        # create folders for evaluation for output
        os.mkdir(eval_working_path)
    if not os.path.exists(out_binary_s_path):
        os.makedirs(out_binary_s_path, exist_ok=True)
    # Copy the source directory to the destination directory
    shutil.copytree(eval_elf_s, os.path.join(out_binary_s_path,
                    os.path.basename(eval_elf_s)), dirs_exist_ok=True)
    # shutil.copytree(eval_elf_s, out_binary_s_path)
    print('prepare for elf_s success')

    for opt_level in target_opt_levels:
        # parent directory for working path
        eval_working_subpath = eval_working_path + opt_level + '/'

        out_uvprojx_path = eval_working_subpath + 'proj/'
        out_binary_ns_path = eval_working_subpath + 'elf_ns/'
        out_metadata_path = eval_working_subpath + 'metadata/'
        out_tmp_path = eval_working_subpath + 'build_tmp/'
        out_log_path = eval_working_subpath + 'eval_log/'
        out_eval_log = out_log_path + 'eval.log'

        check_and_create_directory(eval_working_subpath)
        check_and_create_directory(out_metadata_path)
        check_and_create_directory(out_uvprojx_path)
        check_and_create_directory(out_binary_ns_path)
        check_and_create_directory(out_tmp_path)
        check_and_create_directory(out_log_path)
        check_and_create_file(out_eval_log)

        # start to run evaluation on beebs
        # EvaluationRunnerForBEEBs(uart_port, hw_reset_port, sw_reset_port).beebs_eval_run_all()
        # EvaluationRunnerForBlinky(opt_level, uart_port, hw_reset_port, sw_reset_port).blinky_eval_run_all()
        # EvaluationRunnerForFreeRTOS(opt_level, uart_port, hw_reset_port, sw_reset_port).FreeRTOS_eval_run_all()
        # EvaluationRunnerForLEDTrigger(opt_level, uart_port, hw_reset_port, sw_reset_port).trigger_eval_run_all()
        # EvaluationRunnerForVulfooBOF(opt_level, uart_port, hw_reset_port, sw_reset_port).vulfoo_eval_run_all()
        EvaluationRunnerForVulfooTask(
            opt_level, uart_port, hw_reset_port, sw_reset_port).task_vulfoo_eval_run_all()
