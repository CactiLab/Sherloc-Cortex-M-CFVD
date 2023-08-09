#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311

from eval import *


class UVPROJXBuilderForBlinky(object):
    def __init__(self, proj_name, frequency, in_proj_path_ns, eval_blinky_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, opt_level):
        self.proj_name = f'{proj_name}_{frequency}'
        self.build_proj_name = proj_name
        self.frequency = frequency
        self.in_proj_path_ns = in_proj_path_ns
        self.proj_uvprojx = self.in_proj_path_ns + proj_name + ".uvprojx"
        self.build_obj = in_proj_path_ns + 'Objects/'
        self.eval_blinky_src_path = eval_blinky_src_path
        self.out_uvprojx_path = out_uvprojx_path
        self.out_binary_ns_path = out_binary_ns_path
        self.out_tmp_path = out_tmp_path
        self.opt_level = opt_level

    def build_run(self):
        dst_main = self.in_proj_path_ns + 'main_ns.c'
        src_uvprojx = './blinky/Sherloc_Blinky_ns_' + self.opt_level + '.uvprojx'
        dst_uvprojx = f'{self.in_proj_path_ns}{self.build_proj_name}.uvprojx'
        axf_file_path = f'{self.out_binary_ns_path}{self.proj_name}.axf'
        if not os.path.exists(axf_file_path):
            print(
                "\n---------------------\nprepare for project building...", self.proj_name)
            src_main = f'{self.eval_blinky_src_path}{self.proj_name}_main.c'
            copy_file(src_main, dst_main)
            copy_file(src_uvprojx, dst_uvprojx)
            if os.path.exists(self.build_obj):
                print("clear ", self.build_obj)
                shutil.rmtree(self.build_obj)
            UVPROJXBuilderByName(self.build_proj_name, self.in_proj_path_ns,
                                 self.out_binary_ns_path, self.out_tmp_path, self.proj_name).builder_by_name_run()
        else:
            print(self.proj_name + " has already done.")


class MetadataCreatorForBlinky(object):
    def __init__(self, proj_name, frequency, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        self.proj_name = f'{proj_name}_{frequency}'
        self.frequency = frequency
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path

    def create_metadata_run(self):
        result = "./eval_result_template.csv"
        out_result = self.out_log_path + 'eval_result.csv'
        blinky_result = self.out_log_path + 'blinky_eval_result.csv'
        if not os.path.exists(out_result):
            copy_file(result, out_result)
        print("\n---------------------\create metadata for ", self.proj_name)
        intput = self.proj_name + '.axf'
        filepath = os.path.join(self.out_binary_ns_path, intput)

        object = MetadataCreatorByName(
            filepath, self.out_metadata_path, self.out_log_path)
        object.create_metadata_by_name_run()
        move_file(out_result, blinky_result)


class EmulationForBlinky(object):
    def __init__(self, proj_name, frequency, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.proj_name = f'{proj_name}_{frequency}'
        self.frequency = frequency
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


class EvaluationForBlinky(object):
    def __init__(self, proj_name, frequency, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.proj_name = f'{proj_name}_{frequency}'
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
