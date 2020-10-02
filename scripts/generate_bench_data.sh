#!/usr/bin/bash
GEN=$1
DIR=$2

mkdir -p $DIR

DEFAULT_ROWS='100'
DEFAULT_COLS='100'
DEFAULT_CELL='10'

FNAME=$DEFAULT_ROWS'_'$DEFAULT_COLS'_'$DEFAULT_CELL'.csv'
printf "Generating %s..." $FNAME
$($GEN $DEFAULT_ROWS $DEFAULT_COLS $DEFAULT_CELL > $DIR/$FNAME)
printf "DONE\n"

BENCH_ROWS='1'
for i in {1..7}; do
    BENCH_ROWS+='0'
    FNAME=$BENCH_ROWS'_'$DEFAULT_COLS'_'$DEFAULT_CELL'.csv'
    printf "Generating %s..." $FNAME
    $($GEN $BENCH_ROWS $DEFAULT_COLS $DEFAULT_CELL > $DIR/$FNAME)
    printf "DONE\n"
done

BENCH_COLS='1'
for i in {1..7}; do
    BENCH_COLS+='0'
    FNAME=$DEFAULT_ROWS'_'$BENCH_COLS'_'$DEFAULT_CELL'.csv'
    printf "Generating %s..." $FNAME
    $($GEN $DEFAULT_ROWS $BENCH_COLS $DEFAULT_CELL > $DIR/$FNAME)
    printf "DONE\n"
done

BENCH_CELL='1'
for i in {1..6}; do
    BENCH_CELL+='0'
    FNAME=$DEFAULT_ROWS'_'$DEFAULT_COLS'_'$BENCH_CELL'.csv'
    printf "Generating %s..." $FNAME
    $($GEN $DEFAULT_ROWS $DEFAULT_COLS $BENCH_CELL > $DIR/$FNAME)
    printf "DONE\n"
done