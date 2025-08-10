#!/usr/bin/python3

import os
import sys
import json

json_out_file = 'binlog.json'

execute_virt_start = 0x400000

line_fmt = "objdump -d {} | grep -B 7 'call.*binlog_print' | grep 'mov.*$.*,%ecx'| awk -F'$' '{{print $2}}' | awk -F, '{{print $1}}'"

func_fmt = "objdump -d {} | grep -B 7 'call.*binlog_print' | grep 'mov.*$.*,%edx' | awk -F'$' '{{print $2}}' | awk -F, '{{print $1}}'"

cc_fmt = "objdump -d {} | grep -B 7 'call.*binlog_print' | grep 'mov.*$.*,%r8d' | awk -F'$' '{{print $2}}' | awk -F, '{{print $1}}'"

def elf_parse(file):
    function = os.popen(func_fmt.format(file))
    line = os.popen(line_fmt.format(file))
    fmt = os.popen(cc_fmt.format(file))

    funcs = function.read().split('\n')[:-1]
    lines = line.read().split('\n')[:-1]
    cfmts = fmt.read().split('\n')[:-1]

    function.close()
    line.close()
    fmt.close()

    res = [(int(x, 16), int(y, 16), int(z, 16)) for x, y, z in zip(funcs, lines, cfmts)]

    print(res)
    return res

def elf_read_str(f, o):
    f.seek(o)
    b = f.read(1)
    s = ""
    while b != b'\x00':
        s = s + b.decode()
        b = f.read(1)
    return s

def elf_foff(virt):
    return virt - execute_virt_start if virt > execute_virt_start else virt

def parse_binlog(files):
    print(files)
    argset = []
    for f in files:
        with open(f, 'rb') as fp:
            elf_list = elf_parse(f)
            for x, y, z in elf_list:
                func_foff = elf_foff(x)
                fmt_foff = elf_foff(z)
                argset.append({
                    'function': elf_read_str(fp, func_foff),
                    'line': y,
                    'format': elf_read_str(fp, fmt_foff)
                })
    return argset

def dump_json(argset):
    with open(json_out_file, 'w', encoding="utf-8") as fp:
        json.dump(
            argset,
            fp,
            ensure_ascii = False,
            indent = 4,
            sort_keys = True
        )

def usage():
    print("Usage: binlog_elf_parser.py [..files]\n"
          "\tbinlog_elf_parser.py a.so b.so c.out ...\n")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        usage()
        sys.exit()
    scan_files = sys.argv[1:]
    argset = parse_binlog(scan_files)
    dump_json(argset)
    