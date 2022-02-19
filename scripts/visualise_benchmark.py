#!/usr/bin/python
import pandas as pd
from matplotlib import pyplot as plt
import sys
import math


def get_params(parameter_file):
    f = parameter_file[:-len('.csv')]
    f = f[len('data/benchmark/'):]
    return [int(x) for x in f.split('_')]


def plot_map(axs, f, x, y, label):
    axs.plot(list(map(f, x)), list(map(f, y)), label=label)


if len(sys.argv) != 2:
    print("Expected exactly one argument: <file-name>")
    sys.exit(1)

file_name = sys.argv[1]
raw_benchmark_data = pd.read_csv(file_name)

times = dict()
entries_by_lines = dict()
entries_by_cols = dict()
entries_by_chars = dict()

for i in range(len(raw_benchmark_data.index)):
    row = raw_benchmark_data.iloc[i]
    lines, cols, chars = get_params(row['parameter_file'])
    entries_by_lines[lines] = entries_by_lines.get(lines, []) + [i]
    entries_by_cols[cols] = entries_by_cols.get(cols, []) + [i]
    entries_by_chars[chars] = entries_by_chars.get(chars, []) + [i]
    times[(lines, cols, chars)] = float(row['mean'])

default_lines = min(entries_by_lines.keys())
default_cols = min(entries_by_cols.keys())
default_chars = min(entries_by_chars.keys())

lines = sorted(entries_by_lines.keys())
times_by_lines = [times[(l, default_cols, default_chars)] for l in lines]

cols = sorted(entries_by_cols.keys())
times_by_cols = [times[(default_lines, c, default_chars)] for c in cols]

chars = sorted(entries_by_chars.keys())
times_by_chars = [times[(default_lines, default_cols, c)] for c in chars]

f = lambda x: x # id(x) = x

fig, axs = plt.subplots()

fig.set_dpi(150)

plot_map(axs, f, lines, times_by_lines, "N = Number of lines per file")
plot_map(axs, f, cols, times_by_cols, "N = Number of columns per line")
plot_map(axs, f, chars, times_by_chars, "N = Number of characters per cell")

plt.xlabel("Problem size N")
plt.ylabel("Time taken in seconds")

plt.legend()
plt.savefig(f"{'.'.join(file_name.split('.')[:-1])}.png")