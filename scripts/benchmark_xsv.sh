#!/bin/sh
DIR=$1
BIN=$2
NUM_LINES=$3
OUT_LOG=$4
if [ -z "$4" ]; then 
    printf "Expected 4 arguments: <DATA-DIR> <BINARY> <NUM_LINES> <OUT_LOG>\n"
    exit 1
fi
dout="./temp"
rm -rf $dout
files=$(find $1 -type f | paste -sd ',')
hyperfine \
    "$BIN split -s $NUM_LINES $dout/xsv  {file}" \
    --export-csv $OUT_LOG \
    -m 3 \
    -i \
    --warmup 3 \
    -L file $files \
    --prepare "mkdir -p $dout" \
    --cleanup "rm -rf $dout" \
    --show-output