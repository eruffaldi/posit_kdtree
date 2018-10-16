#!/bin/bash
P=.
$P/nanoflann_any_test list
[ ! -f xTr.bin ] && $P/nanoflann_any_test gentrain -d 10 --count 1000 --minrange 0 --maxrange 1.0 -i xTr.bin
[ ! -f xte.bin ] && $P/nanoflann_any_test gentest -d 10 --count 10 --minrange 0 --maxrange 1.0 -t xTe.bin
# using build from flosting tree
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d 10 -k 10 -T float -f 1 -o xT_xt_10_float_float.json
# using build from flosting ... not
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d 10 -k 10 -T float -o xT_xt_10_float.json
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d 10 -k 10 -T posit8_0 -o xT_xt_10_posit8_0.json
