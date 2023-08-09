#!/usr/bin/env python

import copy
import json

import angr
from capstone import *
from elftools.elf.elffile import ELFFile


class analyze_on_target():
    def __init__(self, filepath, metadata_path):
        self.proj, self.cfg, self.code = self.load_cfg(filepath)
        self.edge_table = self.analyze_cfg()

    def load_cfg(self, filename):
        proj = angr.Project(filename, main_opts={'arch': 'ArchARMCortexM'})
        print(proj.arch)
        print(hex(proj.entry))

        try:
            elf = ELFFile(open(filename, 'rb'))
        except IOError as e:
            print(e)
        code_section = elf.get_section_by_name('ER_ROM')
        if code_section:
            pass
        else:  # For FreeRTOS+MPU
            code_section = elf.get_section_by_name('ER_IROM_NS_PRIVILEGED')

        code = code_section.data()

        # static CFG
        cfg = proj.analyses.CFGFast()
        # cfg = proj.analyses.CFGEmulated(keep_state=True)

        return proj, cfg, code

    def analyze_cfg(self):
        # dynamic CFG

        print("This is the graph:", self.cfg.graph)
        print("It has %d nodes and %d edges" %
              (len(self.cfg.graph.nodes()), len(self.cfg.graph.edges())))

        print(type(self.proj.entry))

        entry_node = self.cfg.get_any_node(self.proj.entry)

        print("There were %d contexts for the entry block" %
              len(self.cfg.get_all_nodes(self.proj.entry)))
        print("Predecessors of the entry point:", entry_node.predecessors)
        print("Successors of the entry point:", entry_node.successors)

        print("Successors (and type of jump)  of the entry point:", [jumpkind + " to " + str(
            node.addr) for node, jumpkind in self.cfg.get_successors_and_jumpkind(entry_node)])

        conn = {}
        conn_pair = {}
        idx = 0

        edge_table = {}
        conn = {}
        conn_pair = {}
        idx = 0
        branch_insns = ["blx", "bx"]

        analyzed_node = []

        # current_node = "__main"
        for edges in self.cfg.graph.edges():
            src_node = edges[0]
            dst_node = edges[1]

            block = self.proj.factory.block(src_node.addr)

            print("--------------------BB start----------------------")
            print('src_node: [0x%x] %s (src_node_size: %x), dst_node: [0x%x] %s (dst_node_size: %x)' % (
                src_node.addr-1, src_node.name, src_node.size, dst_node.addr-1, dst_node.name, dst_node.size))

            insns = block.capstone.insns

            if insns:
                las_insns = insns[-1]
                mnemonic = las_insns.mnemonic
                op_str = las_insns.op_str
                isrc = las_insns.address
                if mnemonic == "blx":
                    ret = isrc + 2
                    conn_pair['type'] = "icall"
                elif mnemonic == "bl":
                    ret = isrc + 4
                if ret == dst_node.addr:
                    print("ret")
                    continue
                if mnemonic in branch_insns:
                    if op_str == "lr":
                        print("ret: bx lr")
                        continue
                    idst = dst_node.addr
                    conn_pair['src_name'] = src_node.name
                    conn_pair['dst_name'] = dst_node.name
                    conn_pair['ins'] = mnemonic + ' ' + op_str
                    conn_pair['src_addr'] = copy.copy('0x%x' % (isrc-1))
                    conn_pair['dst_addr'] = copy.copy('0x%x' % (idst-1))
                    conn[idx] = copy.copy(conn_pair)
                    conn_pair = {}
                    idx = idx + 1
                    print(
                        f'indirect branch: {las_insns}, src: {hex(isrc-1)}, dst: {hex(idst-1)}')

            print("==> insns_list")
            for i in insns:
                # insn: <CsInsn 0x200419 [01d1]: bne #0x20041f>, size: 2, address: 2098201, mnemonic: bne, op_str: #0x20041f
                print(
                    f'insn: {i.insn}, size: {i.size}, address: 0x{i.address:x}, mnemonic: {i.mnemonic}, op_str: {i.op_str}')

            if src_node in analyzed_node:
                pass
            else:
                analyzed_node.append(src_node)

        edge_table = conn
        return edge_table

    def write_json(self, metadata_path):
        # Save analyzed address pairs
        with open(metadata_path, 'w', encoding='utf-8') as outfile:
            json.dump(self.edge_table, outfile, ensure_ascii=False, indent=4)
