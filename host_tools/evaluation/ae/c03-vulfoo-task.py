#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311

import sys
sys.path.append('../')

from eval_beebs import *
from eval_blinky import *
from eval_freertos import *
from eval_trigger import *
from eval_vulfoo import *
from eval import *

eval_elf_s = '../../../host_tools/evaluation/elf_s/'
example_path = '../../../Example/'
eval_working_path = example_path + 'out/eval/'
out_binary_s_path = eval_working_path + 'elf_s/'
uart_port = 'COM4'
hw_reset_port = 'COM6'
sw_reset_port = 'COM5'
mps_image_s = 'E:/SOFTWARE/s.axf'
mps_image_ns = 'E:/SOFTWARE/ns.axf'
mps_metadata = 'E:/SOFTWARE/bs.bin'

def main():
  eval_working_subpath = eval_working_path + 'O3' + '/'
  out_log_path = eval_working_subpath + 'eval_log/'
  out_eval_log = 'eval.log'

  # load sherloc runtime to the board
  print('load sherloc runtime to the board')
  
  SHERLOC_image = eval_elf_s + 'full_rtos.axf'
  BoardOperation('', out_eval_log, SHERLOC_image, mps_image_s, hw_reset_port, sw_reset_port).load_file()

  # load non-secure benchmark to the board
  benchmark_image = eval_working_subpath + 'elf_ns/FreeRTOS_MPU_ns_task_vul.axf'
  BoardOperation('', out_eval_log, benchmark_image, mps_image_ns, hw_reset_port, sw_reset_port).load_file()

  # load medata of the benchmark to the board
  benchmark_metadata = eval_working_subpath + 'metadata/FreeRTOS_MPU_ns_task_novul.bin'
  BoardOperation('', out_eval_log, benchmark_metadata, mps_metadata, hw_reset_port, sw_reset_port).load_file()

# call main
if __name__ == "__main__":
  main()