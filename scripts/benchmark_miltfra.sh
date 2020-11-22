#!/bin/sh
DIR=$1
BIN=$2
NUM_LINES=$3
OUT_LOG=$4
if [ -z "$4" ]; then 
    printf "Expected 4 arguments: benchmark_miltfra.sh <DATA-DIR> <BINARY> <NUM_LINES> <OUT_LOG>\n"
    exit 1
fi
dout="./tmp"
rm -rf $dout
files=$(find $1 -type f | paste -sd ',')
hyperfine \
    "$BIN -l $NUM_LINES {file} -n $dout/split" \
    --export-csv $OUT_LOG \
    -m 1000 \
    -i \
    --warmup 100 \
    -L file $files \
    --prepare "mkdir -p $dout" \
    --cleanup "rm -rf $dout" \
    --show-output