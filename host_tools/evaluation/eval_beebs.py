from eval import *


class UVPROJXCreatorForBEEBS(object):
    def __init__(self, BEEBS_name_list, eval_beebs_src_path, out_uvprojx_path, opt):
        self.tree = ET.parse('uvprojx/Sherloc_ns_template_' + opt + '.uvprojx')
        self.root = self.tree.getroot()
        self.files = self.find_node()
        self.file_list = []
        self.lib_file = ["Sherloc_s_Lib.o",
                         "BEEBS.c", "BEEBS.h", "Secure_Functions.h"]
        self.bench_src_path = eval_beebs_src_path
        self.BEEBS_name_list = BEEBS_name_list
        self.out_uvprojx_path = out_uvprojx_path

    # find the source file group in the .uvprojx file
    def find_node(self):
        for group in self.root.iter('Group'):
            groupname = group.find('GroupName').text
            if groupname == "CMSE Library":
                return group.find('Files')

    # find required source files to build beebs elf file
    def find_source(self, bench_name):
        filepath = self.bench_src_path + bench_name
        source_list = []
        find_flag = 0
        if os.path.exists(filepath):
            for entry in os.listdir(filepath):
                if entry.endswith('.c'):
                    print("find source file", bench_name + "/" + entry)
                    source_list.append(entry)
                    find_flag = 1
        if find_flag == 0:
            print("cannot find source files at", filepath)
            sys.exit()
        return source_list

    # delete source files of previous beebs suite
    def delete_node(self):
        for file in list(self.files):
            filename = file.find('FileName').text
            # print("checking source files", filename)
            if filename not in self.lib_file:
                print("deleting previous files",
                      file.find('FileName').text)
                self.files.remove(file)
        return file

#     <File>
#       <FileName>Sherloc_s_CMSE_Lib.o</FileName>
#       <FileType>3</FileType>
#       <FilePath>..\Sherloc_s\Objects\Sherloc_s_CMSE_Lib.o</FilePath>
#     </File>
    def create_node(self, bench_name, source_list, File):
        for source_file_name in source_list:
            File = ET.Element('File')
            ET.SubElement(File, 'FileName').text = source_file_name
            ET.SubElement(File, 'FileType').text = "1"
            ET.SubElement(File, 'FilePath').text = '../' + \
                self.bench_src_path + bench_name + "/" + source_file_name
            self.files.insert(5, File)

    # generate the uvprojx file for keil to build
    def create_uvprojx(self):
        for bench_name in self.BEEBS_name_list:
            uvprojx_file = self.out_uvprojx_path + bench_name + ".uvprojx"
            if not os.path.exists(uvprojx_file):
                print("\ncreating uvprojCreator for", bench_name)
                file = self.delete_node()
                source_list = self.find_source(bench_name)
                self.create_node(bench_name, source_list, file)
                self.tree.write(uvprojx_file)
                uvprojx_tmp = uvprojx_file+'.bak'
                head = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n"
                with open(uvprojx_file, "r") as read_obj, open(uvprojx_tmp, "w") as write_obj:
                    write_obj.write(head)
                    for line in read_obj:
                        write_obj.write(line)
                # remove original file
                os.remove(uvprojx_file)
                # rename tmp file as the origin al file
                os.rename(uvprojx_tmp, uvprojx_file)
            else:
                print(f'{uvprojx_file} has already done, go to next.')

    def create_header(self):
        for bench_name in self.BEEBS_name_list:
            output_header_file = self.out_uvprojx_path + bench_name + ".h"
            if not os.path.exists(output_header_file):
                print("\ncreating header for", bench_name)
                with open(output_header_file, "w") as metadata_header:
                    metadata_header.write(f'''
#ifndef BEEBS_H
#define BEEBS_H

#define {bench_name.upper()}

#include "support/support.h"

void initialise_benchmark();
int verify_benchmark(int result);
int benchmark();

int BEEBS();


#endif // BEEBS_H
                    ''')
                    metadata_header.close()
            else:
                print(f'{output_header_file} has already done, go to next.')

    def creator_run(self):
        self.create_uvprojx()
        self.create_header()


class UVPROJXBuilderForBEEBS(object):
    def __init__(self, BEEBS_name_list, in_beebs_path_ns, out_uvprojx_path, out_binary_ns_path, out_tmp_path):
        self.BEEBS_name_list = BEEBS_name_list
        self.in_beebs_path_ns = in_beebs_path_ns
        self.proj_uvprojx = self.in_beebs_path_ns + "Sherloc_ns.uvprojx"
        self.proj_header = "../../Sherloc_runtime/evaluation/BEEBS.h"
        self.build_obj = in_beebs_path_ns + 'Objects/'
        self.out_uvprojx_path = out_uvprojx_path
        self.out_binary_ns_path = out_binary_ns_path
        self.out_tmp_path = out_tmp_path

    def builder_preparation(self, bench_name):
        if os.path.exists(self.build_obj):
            print("clear ", self.build_obj)
            shutil.rmtree(self.build_obj)

        if os.path.exists(self.proj_uvprojx):
            print("replace ", self.proj_uvprojx)
            os.remove(self.proj_uvprojx)
        shutil.copy(self.out_uvprojx_path + bench_name +
                    ".uvprojx", self.proj_uvprojx)

        if os.path.exists(self.proj_header):
            print("replace ", self.proj_header)
            os.remove(self.proj_header)
        shutil.copy(self.out_uvprojx_path +
                    bench_name + ".h", self.proj_header)

    def uvprojx_builder(self, bench_name):
        UVPROJXBuilderByName("Sherloc_ns", self.in_beebs_path_ns, self.out_binary_ns_path,
                             self.out_tmp_path, bench_name).builder_by_name_run()

    def build_run(self):
        for bench_name in self.BEEBS_name_list:
            print("\n---------------------\nprepare for project building...", bench_name)
            axf_file = self.out_binary_ns_path + bench_name + '.axf'
            if not os.path.exists(axf_file):
                # bench_name = "dijkstra"
                self.builder_preparation(bench_name)
                self.uvprojx_builder(bench_name)
            else:
                print(axf_file + " has already done, go to next.")


class MetadataCreatorForBEEBS(object):
    def __init__(self, BEEBS_name_list, out_binary_ns_path, out_metadata_path, out_log_path, opt):
        self.BEEBS_name_list = BEEBS_name_list
        self.out_binary_ns_path = out_binary_ns_path
        self.out_metadata_path = out_metadata_path
        self.out_log_path = out_log_path

    def create_metadata_run(self):
        result = "./eval_result_template.csv"
        out_result = self.out_log_path + 'eval_result.csv'
        beebs_result = self.out_log_path + 'beebs_eval_result.csv'
        if os.path.exists(result):
            shutil.copy(result, out_result)
        for bench_name in self.BEEBS_name_list:
            print("\n---------------------\create metadata for ", bench_name)
            intput = bench_name + '.axf'
            filepath = os.path.join(self.out_binary_ns_path, intput)
            MetadataCreatorByName(filepath, self.out_metadata_path,
                                  self.out_log_path).create_metadata_by_name_run()


class EmulationForBEEBS(object):
    def __init__(self, BEEBS_name_list, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.BEEBS_name_list = BEEBS_name_list
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
        BoardOperation('', self.out_eval_log, SHERLOC_image, mps_image,
                       self.hw_reset_port, self.sw_reset_port).load_file()

    def emu_run(self):
        self.load_SHERLOC()
        start = time.time()
        if TEST_ALL == 1:
            for bench_name in self.BEEBS_name_list:
                ret = EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path,
                                       self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
                while ret == "emu":
                    print("continue emulation")
                    ret = EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path,
                                           self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
                EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path, self.out_log_path,
                                 self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        else:
            bench_name = "qrduino"
            ret = EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path,
                                   self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
            print(ret)
            while ret == "emu":
                print("continue emulation")
                ret = EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path,
                                       self.out_log_path, self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
            EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path, self.out_log_path,
                             self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        print("time elapsed: ", time.time() - start)


class EvaluationForBEEBS(object):
    def __init__(self, BEEBS_name_list, out_binary_s_path, out_binary_ns_path, out_metadata_path, out_log_path, eval_level, uart_port, hw_reset_port, sw_reset_port):
        self.BEEBS_name_list = BEEBS_name_list
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
        BoardOperation('', self.out_eval_log, SHERLOC_image, mps_image,
                       self.hw_reset_port, self.sw_reset_port).load_file()

    def eval_run(self):
        self.load_SHERLOC()
        start = time.time()
        for bench_name in self.BEEBS_name_list:
            print("\n---------------------\nrun evaluation for ", bench_name)
            # if there is no available binary and metadata
            EvaluationByName(bench_name, self.sherloc_path, self.out_binary_ns_path, self.out_metadata_path, self.out_log_path,
                             self.eval_level, self.uart_port, self.hw_reset_port, self.sw_reset_port).eval_by_name_run()
        print("time elapsed: ", time.time() - start)
