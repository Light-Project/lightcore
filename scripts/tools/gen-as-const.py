#!/usr/bin/python3
# SPDX-License-Identifier: GPL-2.0-or-later
#

import os
import re
import argparse
import subprocess
import tempfile

def compute_c_consts(sym_data, cc):
    out_lines = []

    for arg in sym_data:
        if isinstance(arg, str):
            if arg == 'START':
                out_lines.append('void\ndummy (void)\n{')
            else:
                out_lines.append(arg)
            continue
        name = arg[0]
        value = arg[1]
        out_lines.append('asm ("/* @@@name@@@%s@@@value@@@%%0@@@end@@@ */" '
                         ': : \"i\" ((long int) (%s)));'
                         % (name, value))
    out_lines.append('}')
    out_lines.append('')
    out_text = '\n'.join(out_lines)

    with tempfile.TemporaryDirectory() as temp_dir:
        c_file_name = os.path.join(temp_dir, 'test.c')
        s_file_name = os.path.join(temp_dir, 'test.s')
        with open(c_file_name, 'w') as c_file:
            c_file.write(out_text)
        # Compilation has to be from stdin to avoid the temporary file
        # name being written into the generated dependencies.
        cmd = ('%s -S -o %s -x c - < %s' % (cc, s_file_name, c_file_name))
        subprocess.check_call(cmd, shell=True)
        consts = {}
        with open(s_file_name, 'r') as s_file:
            for line in s_file:
                match = re.search('@@@name@@@([^@]*)'
                                  '@@@value@@@[^0-9Xxa-fA-F-]*'
                                  '([0-9Xxa-fA-F-]+).*@@@end@@@', line)
                if match:
                    if (match.group(1) in consts
                        and match.group(2) != consts[match.group(1)]):
                        raise ValueError('duplicate constant %s'
                                         % match.group(1))
                    consts[match.group(1)] = match.group(2)
        return consts

def main():
    parser = argparse.ArgumentParser(description='Produce headers of assembly constants.')
    parser.add_argument('--cc', metavar='CC', help='C compiler (including options) to use')
    parser.add_argument('sym_file', help='.sym file to process')
    args = parser.parse_args()
    sym_data = []

    with open(args.sym_file, 'r') as sym_file:
        started = False

        # Pass preprocessor directives through.
        for line in sym_file:
            line = line.strip()
            if line.startswith('#include'):
                sym_data.append(line)

        sym_file.seek(0)
        for line in sym_file:
            line = line.strip()
            if line == '':
                continue
            if line.startswith('#'):
                if not line.startswith('#include'):
                    sym_data.append(line)
                continue
            words = line.split(maxsplit=1)
            if not started:
                sym_data.append('START')
                started = True
            # Separator.
            if words[0] == '--':
                continue
            name = words[0]
            value = words[1] if len(words) > 1 else words[0]
            sym_data.append((name, value))
        if not started:
            sym_data.append('START')

        consts = compute_c_consts(sym_data, args.cc)
        print(''.join('#define %s %s\n' % c for c in sorted(consts.items())), end='')

if __name__ == '__main__':
    main()
