#!/bin/sh
DIR=$1
NUM_LINES=$2
OUT_LOG=$3
if [ -z "$3" ]; then 
    printf "Expected 3 arguments: benchmark_miltfra.sh <DATA-DIR> <NUM_LINES> <OUT_LOG>\n"
    exit 1
fi
dout="./temp"
rm -rf $dout
files=$(find $DIR -type f | paste -sd ',')
make bin/imartingraham
BIN="ruby bin/imartingraham/csv-split.rb"
hyperfine \
    "$BIN -l $NUM_LINES -f {file} -n $dout/split" \
    --export-csv $OUT_LOG \
    -m 3 \
    -i \
    --warmup 3 \
    -L file $files \
    --prepare "mkdir -p $dout" \
    --cleanup "rm -rf $dout" \
    --show-output
