import os
import math
import pandas as pd
from scipy.stats import gmean
from tabulate import tabulate
from eval import *

'''
usage: python .\result.py > .\result.log
input: all log files
output: csv file that contains the evaluation results
'''


def format_integer(number):
    if math.isnan(number):
        return number
    return format(int(number), ',')


def clean_integer(number):
    return int(number.replace(',', ''))


def get_elf_size(file):
    if os.path.exists(file):
        return os.path.getsize(file)


def update_IBT(proj_metadata_file):
    obj = MetaDataRefresh(bin_file_path=proj_metadata_file)
    bt_header, _, _, _ = obj.parse_metadata_from_file()
    return int(bt_header.IBT_size)


def get_baseline(baseline_record):
    return int(baseline_record[0].split(':')[1].strip())


def get_dict(data):
    data_parts = [part.strip() for part in data.split(',')]
    data_dict = {}

    for part in data_parts:
        k, v = part.split(':')
        key = k.strip()
        value = int(v.strip())
        data_dict[key] = value
    return data_dict


def read_oneline(file_name):
    records = []
    if os.path.exists(file_name):
        with open(file_name, 'r') as file:
            lines = file.readlines()
        for i, line in enumerate(lines):
            if i == 3:
                return get_dict(line.strip())
    else:
        print(file_name + " dose not exist!")
    return records


def read_first_line(file_name):
    records = []
    if os.path.exists(file_name):
        with open(file_name, 'r') as file:
            line = file.readline()
            return get_dict(line.strip())
    else:
        print(file_name + " dose not exist!")
    return records


def read_file(file_name):
    records = []
    if os.path.exists(file_name):
        with open(file_name, "r") as file:
            lines = file.readlines()

        lines.pop(0)  # Skip the first line

        averages = {}
        count = 0

        for line in lines:
            count += 1
            pairs = line.strip().split(", ")

            for pair in pairs:
                key, value = pair.split(": ")
                if key not in averages:
                    averages[key] = 0
                averages[key] += int(value)

        for key in averages:
            averages[key] = int(averages[key]/count)

        return averages
    else:
        print(file_name + " dose not exist!")

    return records


def calculate_overhead(baseline, result_dict):
    overhead = {}
    for key in result_dict:
        overhead[key] = round((result_dict[key]/baseline)*100, 2)
    return overhead


class BEEBsResult(object):
    def __init__(self):
        self.opt_levels = ['O3', 'Oz']

    def fresh_cvs_beebs(self, opt_level):
        work_path = '../../Example/out/eval/'
        log_path = work_path + opt_level + '/eval_log/'
        result_beebs = log_path + 'beebs_eval_result.csv'
        final_beebs = log_path + 'beebs_eval_result_final.csv'

        elf_path = work_path + opt_level + '/elf_ns/'
        metadata_path = work_path + opt_level + '/metadata/'

        # "matmult_int", "stb_perlin",
        BEEBS_name_list = ['bubblesort', 'crc32', 'dijkstra', 'edn', 'fasta', 'frac', 'levenshtein', 'nbody', 'ndes', 'nettle_aes', 'picojpeg', 'qrduino',
                           'rijndael', 'sglib_arraybinsearch', 'sglib_dllist', 'sglib_hashtable', 'sglib_listsort', 'sglib_queue', 'sglib_rbtree', 'st', 'whetstone']

        df_beebs = pd.read_csv(result_beebs)

        for beebs_name in BEEBS_name_list:
            print(f'{beebs_name} {opt_level}')
            baseline_file = log_path + beebs_name + '_baseline.log'
            eval_ibt_baseline = log_path + beebs_name + '_ibt_baseline_no_systick.log'
            eval_ibt_checking = log_path + beebs_name + '_ibt_no_systick.log'
            eval_full_file = log_path + beebs_name + '_full_no_irq.log'
            eval_read_file = log_path + beebs_name + '_sherloc_read.log'
            eval_ins_file = log_path + beebs_name + '_ins.log'
            elf_name = elf_path + beebs_name + '.axf'
            proj_metadata_file = metadata_path + beebs_name + '.bin'

            baseline_dict = read_oneline(baseline_file)
            full_dict = read_oneline(eval_full_file)
            read_dict = read_oneline(eval_read_file)
            ins_dict = read_oneline(eval_ins_file)
            ibt_baseline = read_oneline(eval_ibt_baseline)
            ibt_checking = read_oneline(eval_ibt_checking)

            # enter [backward_edge|forward_edge|ins_identification|sherloc_read] exit
            baseline = baseline_dict['NONE']
            sherloc_detection = full_dict['sherloc_detection']
            result_dict = {}
            result_dict['SHERLOC'] = full_dict['EVAL'] - baseline
            result_dict['enter_exit'] = full_dict['enter_exit'] - baseline
            result_dict['read'] = read_dict['sherloc_read']
            result_dict['ins_identification'] = ins_dict['sherloc_ins_identification'] - \
                result_dict['read']
            result_dict['forward'] = ibt_checking['ibt_check'] - \
                ibt_baseline['ibt_check']

            result_dict['ss'] = sherloc_detection - result_dict['read'] - \
                result_dict['forward'] - result_dict['ins_identification']

            print("result_dict: ", result_dict)

            eval_overhead = calculate_overhead(baseline, result_dict)
            print("eval_overhead: ", eval_overhead)

            # write result to the table
            df_beebs.loc[df_beebs['name'] ==
                         beebs_name, "opt_level"] = opt_level
            df_beebs.loc[df_beebs['name'] == beebs_name,
                         "elf_size"] = get_elf_size(elf_name)
            df_beebs.loc[df_beebs['name'] == beebs_name, "IBT_size"] = update_IBT(
                proj_metadata_file) * 4
            df_beebs.loc[df_beebs['name'] == beebs_name, "AIT_size"] = 3 * 4
            df_beebs.loc[df_beebs['name'] == beebs_name, "baseline"] = baseline
            df_beebs.loc[df_beebs['name'] == beebs_name,
                         "SHERLOC"] = result_dict['SHERLOC']
            for item in eval_overhead:
                df_beebs.loc[df_beebs['name'] == beebs_name,
                             'overhead_'+item] = eval_overhead[item]
            df_beebs.to_csv(final_beebs, index=False)
        df_beebs.loc[df_beebs['name'] == 'min', "opt_level"] = opt_level
        df_beebs.loc[df_beebs['name'] == 'max', "opt_level"] = opt_level
        df_beebs.loc[df_beebs['name'] == 'mean', "opt_level"] = opt_level

        integer_column = ['elf_size', 'icall', 'ibranch', 'dbranch',
                          'dcall', 'ret', 'irq', 'baseline', 'SHERLOC', 'baseline']

        for column in df_beebs.columns:
            if column == 'name' or column == 'opt_level':
                continue

            column_data = df_beebs[column]
            mean_value = round(column_data.mean(), 2)
            min_value = round(column_data.min(), 2)
            max_value = round(column_data.max(), 2)
            df_beebs.loc[df_beebs['name'] == 'mean', column] = mean_value
            df_beebs.loc[df_beebs['name'] == 'min', column] = min_value
            df_beebs.loc[df_beebs['name'] == 'max', column] = max_value

            if column in integer_column:
                df_beebs[column] = df_beebs[column].apply(format_integer)

            df_beebs.to_csv(final_beebs, index=False)

    def beebs_csv(self):
        # Read the CSV file (replace 'file.csv' with your file's name)
        csv_file1 = '../../Example/out/eval/O3/eval_log/beebs_eval_result_final.csv'
        csv_file2 = '../../Example/out/eval/Oz/eval_log/beebs_eval_result_final.csv'

        # Read the CSV files into pandas DataFrames
        df_beebs1 = pd.read_csv(csv_file1)
        df_beebs2 = pd.read_csv(csv_file2)

        # Merge the two DataFrames
        merged_df_beebs = pd.concat([df_beebs1, df_beebs2], ignore_index=True)

        # Sort the merged DataFrame by the first column
        first_column_name = merged_df_beebs.columns[0]
        sorted_df_beebs = merged_df_beebs.sort_values(by=first_column_name)

        # Convert the DataFrame to a LaTeX table
        latex_table = tabulate(sorted_df_beebs, tablefmt='latex',
                               headers='keys', showindex=False)

        # Write the LaTeX table to a file (replace 'output.tex' with your desired file name)
        with open('beebs_output.tex', 'w') as file:
            file.write(latex_table)

    def run(self):
        print('processing beebs eval results...')
        for opt in self.opt_levels:
            self.fresh_cvs_beebs(opt)
        self.beebs_csv()
        print('\n')


class BlinkyResult(object):
    def __init__(self):
        self.opt_levels = ['O3', 'Oz']

    def fresh_cvs_blinky(self, opt_level):
        work_path = '../../Example/out/eval/'
        log_path = work_path + opt_level + '/eval_log/'
        final_blinky = log_path + 'blinky_eval_result_final.csv'
        result_blinky = log_path + 'blinky_eval_result.csv'

        elf_path = work_path + opt_level + '/elf_ns/'
        metadata_path = work_path + opt_level + '/metadata/'

        Blinky_list = ['Sherloc_Blinky_ns_100ms', 'Sherloc_Blinky_ns_10ms']

        df_blinky = pd.read_csv(result_blinky)

        for blinky_instance in Blinky_list:
            print(f'{blinky_instance} {opt_level}')
            working_log_path = log_path + blinky_instance
            baseline_file = f'{working_log_path}_baseline_blinky.log'
            eval_full_file = f'{working_log_path}_full_blinky.log'
            eval_read_file = f'{working_log_path}_sherloc_read_systick.log'
            eval_ins_file = f'{working_log_path}_ins_systick.log'
            eval_ibt_baseline = f'{working_log_path}_ibt_baseline_systick.log'
            eval_ibt = f'{working_log_path}_ibt_systick.log'
            elf_name = elf_path + blinky_instance + '.axf'
            proj_metadata_file = metadata_path + blinky_instance + '.bin'

            baseline_dict = read_file(baseline_file)

            full_dict = read_first_line(eval_full_file)
            read_dict = read_first_line(eval_read_file)
            ins_dict = read_file(eval_ins_file)

            ibt_baseline_dict = read_first_line(eval_ibt_baseline)
            ibt_dict = read_first_line(eval_ibt)

            # enter [backward_edge|forward_edge|ins_identification|sherloc_read] exit
            baseline = baseline_dict['NONE']
            sherloc_detection = full_dict['sherloc_detection']
            result_dict = {}
            result_dict['SHERLOC'] = full_dict['EVAL'] - baseline
            result_dict['enter_exit'] = full_dict['enter_exit'] - baseline
            result_dict['read'] = read_dict['sherloc_read']
            result_dict['ins_identification'] = ins_dict['sherloc_ins_identification'] - \
                result_dict['read']
            result_dict['forward'] = ibt_dict['ibt_check'] - \
                ibt_baseline_dict['ibt_check']
            result_dict['ss_IRQ'] = sherloc_detection - result_dict['read'] - \
                result_dict['forward'] - \
                result_dict['ins_identification']

            print(f'result_dict: {result_dict}')

            eval_overhead = calculate_overhead(baseline, result_dict)
            print(f'eval_overhead: {eval_overhead}')

            # write result to the table
            df_blinky.loc[df_blinky['name'] ==
                          blinky_instance, "opt_level"] = opt_level
            df_blinky.loc[df_blinky['name'] == blinky_instance,
                          "elf_size"] = get_elf_size(elf_name)
            df_blinky.loc[df_blinky['name'] == blinky_instance, "IBT_size"] = update_IBT(
                proj_metadata_file) * 4
            df_blinky.loc[df_blinky['name'] ==
                          blinky_instance, "AIT_size"] = 3 * 4
            df_blinky.loc[df_blinky['name'] ==
                          blinky_instance, "baseline"] = baseline
            df_blinky.loc[df_blinky['name'] == blinky_instance,
                          "SHERLOC"] = result_dict['SHERLOC']
            for item in eval_overhead:
                df_blinky.loc[df_blinky['name'] == blinky_instance,
                              'overhead_'+item] = eval_overhead[item]
            df_blinky.to_csv(final_blinky, index=False)

        df_blinky.loc[df_blinky['name'] == 'min', "opt_level"] = opt_level
        df_blinky.loc[df_blinky['name'] == 'max', "opt_level"] = opt_level
        df_blinky.loc[df_blinky['name'] == 'mean', "opt_level"] = opt_level

        integer_column = ['elf_size', 'icall', 'ibranch', 'dbranch',
                          'dcall', 'ret', 'irq', 'baseline', 'SHERLOC', 'baseline']

        for column in df_blinky.columns:
            if column == 'name' or column == 'opt_level':
                continue
            column_data = df_blinky[column]
            mean_value = round(column_data.mean(), 2)
            min_value = round(column_data.min(), 2)
            max_value = round(column_data.max(), 2)
            df_blinky.loc[df_blinky['name'] == 'mean', column] = mean_value
            df_blinky.loc[df_blinky['name'] == 'min', column] = min_value
            df_blinky.loc[df_blinky['name'] == 'max', column] = max_value

            if column in integer_column:
                df_blinky[column] = df_blinky[column].apply(format_integer)

            df_blinky.to_csv(final_blinky, index=False)

    def blinky_csv(self):
        csv_file_blinky_O3 = '../../Example/out/eval/O3/eval_log/blinky_eval_result_final.csv'
        csv_file_blinky_Oz = '../../Example/out/eval/Oz/eval_log/blinky_eval_result_final.csv'

        # Read the CSV files into pandas DataFrames
        df_blinky1 = pd.read_csv(csv_file_blinky_O3)
        df_blinky2 = pd.read_csv(csv_file_blinky_Oz)

        # Merge the two DataFrames
        merged_df_blinky = pd.concat(
            [df_blinky1, df_blinky2], ignore_index=True)

        # Sort the merged DataFrame by the first column
        first_column_name = merged_df_blinky.columns[0]
        sorted_df_blinky = merged_df_blinky.sort_values(by=first_column_name)

        # Convert the DataFrame to a LaTeX table
        latex_table = tabulate(sorted_df_blinky, tablefmt='latex',
                               headers='keys', showindex=False)

        # Write the LaTeX table to a file (replace 'output.tex' with your desired file name)
        with open('blinky_output.tex', 'w') as file:
            file.write(latex_table)

    def run(self):
        print('processing blinky eval results...')
        for opt in self.opt_levels:
            self.fresh_cvs_blinky(opt)
        self.blinky_csv()
        print('\n')


class FreeRTOSResult(object):
    def __init__(self):
        self.opt_levels = ['O3', 'Oz']

    def fresh_cvs_freertos(self, opt_level):
        work_path = '../../Example/out/eval/'
        log_path = work_path + opt_level + '/eval_log/'
        final_freertos = log_path + 'freertos_eval_result_final.csv'
        result_freertos = log_path + 'freertos_eval_result.csv'

        elf_path = work_path + opt_level + '/elf_ns/'
        metadata_path = work_path + opt_level + '/metadata/'

        FreeRTOS_list = ['FreeRTOS_MPU_ns_1']

        df_freertos = pd.read_csv(result_freertos)

        for freertos_instance in FreeRTOS_list:
            print(f'{freertos_instance} {opt_level}')
            working_log_path = log_path + freertos_instance
            baseline_file = f'{working_log_path}_baseline_rtos.log'
            eval_full_file = f'{working_log_path}_full_rtos.log'
            eval_read_file = f'{working_log_path}_read_rtos.log'
            eval_ins_file = f'{working_log_path}_ins_rtos.log'
            eval_ibt_baseline_freertos = f'{working_log_path}_ibt_freertos_baseline.log'
            eval_ibt_freertos = f'{working_log_path}_ibt_freertos.log'
            elf_name = elf_path + freertos_instance + '.axf'
            proj_metadata_file = metadata_path + freertos_instance + '.bin'

            baseline_dict = read_first_line(baseline_file)
            full_dict = read_first_line(eval_full_file)
            read_dict = read_first_line(eval_read_file)
            ins_dict = read_first_line(eval_ins_file)
            ibt_baseline_dict = read_first_line(eval_ibt_baseline_freertos)
            ibt_freertos_dict = read_first_line(eval_ibt_freertos)

            # enter [backward_edge|forward_edge|ins_identification|sherloc_read] exit
            baseline = baseline_dict['NONE']
            result_dict = {}
            result_dict['SHERLOC'] = full_dict['EVAL'] - baseline
            result_dict['enter_exit'] = full_dict['enter_exit'] - baseline
            result_dict['read'] = read_dict['sherloc_read']
            result_dict['ins_identification'] = ins_dict['sherloc_ins_identification'] - \
                result_dict['read']
            result_dict['forward'] = ibt_freertos_dict['ibt_check'] - \
                ibt_baseline_dict['ibt_check']
            result_dict['ss'] = result_dict['SHERLOC'] - result_dict['enter_exit'] - \
                result_dict['read'] - result_dict['ins_identification'] - \
                result_dict['forward']

            print("result_dict: ", result_dict)

            eval_overhead = calculate_overhead(baseline, result_dict)
            print("eval_overhead: ", eval_overhead)

            # write result to the table
            df_freertos.loc[df_freertos['name'] ==
                            freertos_instance, "opt_level"] = opt_level
            if os.path.exists(elf_name):
                df_freertos.loc[df_freertos['name'] == freertos_instance,
                                "elf_size"] = get_elf_size(elf_name)
            else:
                print("elf file not exist")
            df_freertos.loc[df_freertos['name'] == freertos_instance, "IBT_size"] = update_IBT(
                proj_metadata_file) * 4
            df_freertos.loc[df_freertos['name'] ==
                            freertos_instance, "AIT_size"] = 3 * 4
            df_freertos.loc[df_freertos['name'] ==
                            freertos_instance, "baseline"] = baseline
            df_freertos.loc[df_freertos['name'] == freertos_instance,
                            "SHERLOC"] = result_dict['SHERLOC']
            for item in eval_overhead:
                df_freertos.loc[df_freertos['name'] == freertos_instance,
                                'overhead_'+item] = eval_overhead[item]
            df_freertos.to_csv(final_freertos, index=False)

        df_freertos.loc[df_freertos['name'] == 'min', "opt_level"] = opt_level
        df_freertos.loc[df_freertos['name'] == 'max', "opt_level"] = opt_level
        df_freertos.loc[df_freertos['name'] == 'mean', "opt_level"] = opt_level

        integer_column = ['elf_size', 'SHERLOC', 'baseline']

        for column in df_freertos.columns:
            if column == 'name' or column == 'opt_level':
                continue
            column_data = df_freertos[column]
            mean_value = round(column_data.mean(), 2)
            min_value = round(column_data.min(), 2)
            max_value = round(column_data.max(), 2)
            df_freertos.loc[df_freertos['name'] == 'mean', column] = mean_value
            df_freertos.loc[df_freertos['name'] == 'min', column] = min_value
            df_freertos.loc[df_freertos['name'] == 'max', column] = max_value

            if column in integer_column:
                df_freertos[column] = df_freertos[column].apply(format_integer)

            df_freertos.to_csv(final_freertos, index=False)

    def freertos_csv(self):
        csv_file_freertos_O3 = '../../Example/out/eval/O3/eval_log/freertos_eval_result_final.csv'
        csv_file_freertos_Oz = '../../Example/out/eval/Oz/eval_log/freertos_eval_result_final.csv'

        # Read the CSV files into pandas DataFrames
        df_freertos1 = pd.read_csv(csv_file_freertos_O3)
        df_freertos2 = pd.read_csv(csv_file_freertos_Oz)

        # Merge the two DataFrames
        merged_df_freertos = pd.concat(
            [df_freertos1, df_freertos2], ignore_index=True)

        # Sort the merged DataFrame by the first column
        first_column_name = merged_df_freertos.columns[0]
        sorted_df_freertos = merged_df_freertos.sort_values(
            by=first_column_name)

        # Convert the DataFrame to a LaTeX table
        latex_table = tabulate(sorted_df_freertos, tablefmt='latex',
                               headers='keys', showindex=False)

        # Write the LaTeX table to a file (replace 'output.tex' with your desired file name)
        with open('freertos_output.tex', 'w') as file:
            file.write(latex_table)

    def run(self):
        print('processing freertos eval results...')
        for opt in self.opt_levels:
            self.fresh_cvs_freertos(opt)
        self.freertos_csv()
        print('\n')


class FreeRTOSTriggerResult(object):
    def __init__(self):
        self.opt_levels = ['O3', 'Oz']
        self.work_path = '../../Example/out/eval/'

    def get_freertos_mpu_trigger(self, opt_level):
        log_path = self.work_path + opt_level + '/eval_log/'
        trigger_file = log_path + 'trigger_eval_result.csv'
        mpu_trigger_result_file = log_path + 'freertos_eval_result.csv'

        df_mpu_trigger = pd.read_csv(mpu_trigger_result_file)
        df_trigger = pd.read_csv(trigger_file)
        # get the row that the name is 'FreeRTOS_MPU_ns_1'
        df_trigger = df_trigger._append(
            df_mpu_trigger[df_mpu_trigger['name'] == 'FreeRTOS_MPU_ns_1'])
        # insert this column to the first csv
        df_trigger.to_csv(trigger_file, index=False)

    def fresh_csv_freertos_trigger(self, opt_level):
        # opt_level = 'Oz'
        # mpu trigger is done when running evaluation for FreeRTOS, so we only need to get the results and metadata from it
        log_path = self.work_path + opt_level + '/eval_log/'
        final_trigger = log_path + 'trigger_eval_result_final.csv'
        result_trigger = log_path + 'trigger_eval_result.csv'

        # old_final_trigger = log_path + get_timestamp() + '_trigger_eval_result_final.csv'
        elf_path = self.work_path + opt_level + '/elf_ns/'
        metadata_path = self.work_path + opt_level + '/metadata/'

        FreeRTOS_list = ['FreeRTOS_MPU_ns_1', 'FreeRTOS_MPU_ns_led']

        df_trigger = pd.read_csv(result_trigger)

        for freertos_instance in FreeRTOS_list:
            print(f'{freertos_instance} {opt_level}')
            # name in the csv file
            working_log_path = log_path + freertos_instance
            if 'led' in freertos_instance:
                item_name = f'{working_log_path}_led'
            else:
                item_name = f'{working_log_path}_MPU'

            baseline_file = f'{working_log_path}_baseline_rtos.log'
            eval_trigger_result_file = f'{item_name}_trigger.log'
            trigger_read_file = f'{item_name}_trigger_read.log'
            trigger_ins_file = f'{item_name}_trigger_ins.log'
            trigger_ibt_file = f'{item_name}_trigger_IBT.log'
            trigger_ibt_baseline_file = f'{item_name}_trigger_IBT_baseline.log'

            elf_name = elf_path + freertos_instance + '.axf'
            proj_metadata_file = metadata_path + freertos_instance + '.bin'

            baseline_dict = read_first_line(baseline_file)

            trigger_result_dict = read_first_line(eval_trigger_result_file)
            trigger_read_dict = read_first_line(trigger_read_file)
            trigger_ins_dict = read_first_line(trigger_ins_file)
            trigger_ibt_dict = read_first_line(trigger_ibt_file)
            trigger_ibt_baseline_dict = read_first_line(
                trigger_ibt_baseline_file)

            # enter [backward_edge|forward_edge|ins_identification|sherloc_read] exit
            baseline = baseline_dict['NONE']
            result_dict = {}
            result_dict['enter_exit'] = trigger_result_dict['enter_exit'] - baseline
            result_dict['SHERLOC'] = trigger_result_dict['Trigger'] - baseline
            result_dict['read'] = trigger_read_dict['sherloc_read']
            result_dict['ins_identification'] = trigger_ins_dict['sherloc_ins_identification'] - \
                result_dict['read']
            result_dict['ibt'] = trigger_ibt_dict['Trigger'] - \
                trigger_ibt_baseline_dict['Trigger']
            result_dict['validation'] = result_dict['SHERLOC'] - \
                result_dict['enter_exit'] - result_dict['read']

            print("result_dict: ", result_dict)

            eval_overhead = calculate_overhead(baseline, result_dict)
            print("eval_overhead: ", eval_overhead)

            # write result to the table
            df_trigger.loc[df_trigger['name'] ==
                           item_name, "opt_level"] = opt_level
            if os.path.exists(elf_name):
                df_trigger.loc[df_trigger['name'] == item_name,
                               "elf_size"] = get_elf_size(elf_name)
            else:
                print("elf file not exist")
            df_trigger.loc[df_trigger['name'] == item_name, "IBT_size"] = update_IBT(
                proj_metadata_file) * 4
            df_trigger.loc[df_trigger['name'] ==
                           item_name, "AIT_size"] = 3 * 4
            df_trigger.loc[df_trigger['name'] ==
                           item_name, "baseline"] = baseline
            df_trigger.loc[df_trigger['name'] == item_name,
                           "SHERLOC"] = result_dict['SHERLOC']
            for item in eval_overhead:
                df_trigger.loc[df_trigger['name'] == item_name,
                               'overhead_'+item] = eval_overhead[item]
            df_trigger.to_csv(final_trigger, index=False)

        df_trigger.loc[df_trigger['name'] == 'min', "opt_level"] = opt_level
        df_trigger.loc[df_trigger['name'] == 'max', "opt_level"] = opt_level
        df_trigger.loc[df_trigger['name'] == 'mean', "opt_level"] = opt_level

        integer_column = ['elf_size', 'icall', 'ibranch', 'dbranch',
                          'dcall', 'ret', 'irq', 'SHERLOC', 'baseline', 'Trigger']

        for column in df_trigger.columns:
            if column == 'name' or column == 'opt_level':
                continue
            column_data = df_trigger[column]
            mean_value = round(column_data.mean(), 2)
            min_value = round(column_data.min(), 2)
            max_value = round(column_data.max(), 2)
            df_trigger.loc[df_trigger['name'] == 'mean', column] = mean_value
            df_trigger.loc[df_trigger['name'] == 'min', column] = min_value
            df_trigger.loc[df_trigger['name'] == 'max', column] = max_value

            if column in integer_column:
                df_trigger[column] = df_trigger[column].apply(format_integer)

            df_trigger.to_csv(final_trigger, index=False)

    def freertos_trigger_csv(self):
        csv_file_trigger_O3 = '../../Example/out/eval/O3/eval_log/trigger_eval_result_final.csv'
        csv_file_trigger_Oz = '../../Example/out/eval/Oz/eval_log/trigger_eval_result_final.csv'

        # Read the CSV files into pandas DataFrames
        df_trigger1 = pd.read_csv(csv_file_trigger_O3)
        df_trigger2 = pd.read_csv(csv_file_trigger_Oz)

        # Merge the two DataFrames
        merged_df_trigger = pd.concat(
            [df_trigger1, df_trigger2], ignore_index=True)

        # Sort the merged DataFrame by the first column
        first_column_name = merged_df_trigger.columns[0]
        sorted_df_trigger = merged_df_trigger.sort_values(
            by=first_column_name)

        # Convert the DataFrame to a LaTeX table
        latex_table = tabulate(sorted_df_trigger, tablefmt='latex',
                               headers='keys', showindex=False)

        # Write the LaTeX table to a file (replace 'output.tex' with your desired file name)
        with open('trigger_output.tex', 'w') as file:
            file.write(latex_table)

    def run(self):
        print('processing event-trigger eval results...')
        for opt in self.opt_levels:
            self.get_freertos_mpu_trigger(opt)
            self.fresh_csv_freertos_trigger(opt)
        self.freertos_trigger_csv()
        print('\n')


BEEBsResult().run()
BlinkyResult().run()
FreeRTOSResult().run()
FreeRTOSTriggerResult().run()
