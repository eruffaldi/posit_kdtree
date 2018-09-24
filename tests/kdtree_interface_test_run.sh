#!/bin/bash
P=.
$P/nanoflann_any_test list
[ ! -f xT.bin ] $P/nanoflann_any_test gentrain -d 10 --count 1000 --minrange 0 --maxrange 1.0 -i xT.bin
[ ! -f xt.bin ] $P/nanoflann_any_test gentest -d 10 --count 10 --minrange 0 --maxrange 1.0 -t xt.bin
$P/nanoflann_any_test run -i xT.bin -t xt.bin -d 10 -k 10 -T float -f -o xT_xt_10_float_float.json
$P/nanoflann_any_test run -i xT.bin -t xt.bin -d 10 -k 10 -T float -o xT_xt_10_float.json
