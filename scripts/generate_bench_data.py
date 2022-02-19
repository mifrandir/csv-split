#!/usr/bin/python
import sys
import os
import subprocess


def get_file_name(rows, columns, chars):
    return f"{rows}_{columns}_{chars}.csv"


def generate_file(binary_path, rows, columns, chars, output_dir):
    return subprocess.run([
        "sh", "-c",
        f"{binary_path} {rows} {columns} {chars} > {output_dir}/{get_file_name(rows, columns,chars)}"
    ])


if len(sys.argv) != 3:
    print("Expected exactly two arguments: <binary> <output-directory>")
    sys.exit(1)

GEN = sys.argv[1]
DIR = sys.argv[2]

row_min, row_max, row_step = 100, 10001, 100
col_min, col_max, col_step = 100, 10001, 100
chr_min, chr_max, chr_step = 100, 10001, 100

if not os.path.isdir(DIR):
    os.mkdir(DIR)

for row in range(row_min, row_max, row_step):
    generate_file(GEN, row, col_min, chr_min, DIR)
for col in range(col_min, col_max, col_step):
    generate_file(GEN, row_min, col, chr_min, DIR)
for char in range(chr_min, chr_max, chr_step):
    generate_file(GEN, row_min, col_min, char, DIR)