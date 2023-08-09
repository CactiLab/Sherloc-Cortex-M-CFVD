#! C:\\Users\\minta\\AppData\\Local\\Programs\\Python\\Python311

from eval import *


class UVPROJXBuilderForVulfoo(object):
    def __init__(self, proj_name, task_number, in_proj_path_ns, vulfoo_src_path, out_uvprojx_path, out_binary_ns_path, out_tmp_path, opt_level):
        self.proj_name = f'{proj_name}_{task_number}'
        self.build_proj_name = proj_name
        self.task_number = task_number
        self.in_proj_path_ns = in_proj_path_ns
        self.proj_uvprojx = self.in_proj_path_ns + proj_name + ".uvprojx"
        self.build_obj = in_proj_path_ns + 'Objects/'
        self.vulfoo_src_path = vulfoo_src_path
        self.out_uvprojx_path = out_uvprojx_path
        self.out_binary_ns_path = out_binary_ns_path
        self.out_tmp_path = out_tmp_path
        self.opt_level = opt_level

    def build_run(self):
        dst_main = self.in_proj_path_ns + 'main_ns.c'
        src_uvprojx = self.vulfoo_src_path + \
            'Vulfoo_FreeRTOS_MPU_ns_' + self.opt_level + '.uvprojx'
        dst_uvprojx = f'{self.in_proj_path_ns}{self.build_proj_name}.uvprojx'
        axf_file_path = f'{self.out_binary_ns_path}{self.proj_name}.axf'
        if not os.path.exists(axf_file_path):
            print(
                "\n---------------------\nprepare for project building...", self.proj_name)
            src_main = f'{self.vulfoo_src_path}{self.proj_name}_main.c'
            copy_file(src_main, dst_main)
            copy_file(src_uvprojx, dst_uvprojx)
            if os.path.exists(self.build_obj):
                print("clear ", self.build_obj)
                shutil.rmtree(self.build_obj)
            UVPROJXBuilderByName(self.build_proj_name, self.in_proj_path_ns,
                                 self.out_binary_ns_path, self.out_tmp_path, self.proj_name).builder_by_name_run()
        else:
            print(self.proj_name + " has already done.")


class MetadataCreatorForVulfoo(object):
    def __init__(self, proj_name, task_number, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        self.proj_name = f'{proj_name}_{task_number}'
        self.task_number = task_number
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path

    def create_metadata_run(self):
        result = "./eval_result_template.csv"
        out_result = self.out_log_path + 'eval_result.csv'
        trigger_result = self.out_log_path + 'trigger_eval_result.csv'
        if not os.path.exists(out_result):
            copy_file(result, out_result)
        print("\n---------------------\create metadata for ", self.proj_name)
        intput = self.proj_name + '.axf'
        filepath = os.path.join(self.out_binary_ns_path, intput)

        object = MetadataCreatorByName(
            filepath, self.out_metadata_path, self.out_log_path)
        object.create_metadata_by_name_run()
        move_file(out_result, trigger_result)
