#!/bin/bash

if [ "$#" != 3 ]; then
    echo "usage $0 <block size> <processes count> <iteration count>" && exit 1
fi

B_SIZE=$1
P_COUNT=$2
I_COUNT=$3

erl -pa ebin -eval "{ok, S} = porttest:test_suite_new($P_COUNT, $B_SIZE), porttest:run_test_suite(S, $I_COUNT), init:stop(0)." -noshell || exit 1

echo "OK"
