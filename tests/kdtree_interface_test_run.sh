#!/bin/bash
P=.
$P/nanoflann_any_test list
DIM=3
KNN=5
TREEPTS=1000
TESTPTS=10
#file is of floats
[ ! -f xTr.bin ] && $P/nanoflann_any_test gentrain -d $DIM --count $TREEPTS --minrange 0 --maxrange 1000.0 -i xTr.bin
[ ! -f xte.bin ] && $P/nanoflann_any_test gentest -d $DIM --count $TESTPTS --minrange 0 --maxrange 1000.0 -t xTe.bin
$P/nanoflann_any_test dumpfile -i xTe.bin -d $DIM > xTe.bin.txt
$P/nanoflann_any_test dumpfile -i xTr.bin -d $DIM > xTr.bin.txt
# using build from flosting tree
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d 10 -k 10 -T float -f 1 -o xT_xt_10_float_float.json
# using build from flosting ... not
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T float -o xT_xt_10_float.json
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T double -o xT_xt_10_double.json
diff xT_xt_10_float.json xT_xt_10_double.json

$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T float -f 1 -o xT_xt_10_floatF.json
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T double -f 1 -o xT_xt_10_doubleF.json
diff xT_xt_10_floatF.json xT_xt_10_doubleF.json

#TODO FIXME
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T float -f 1 -o xT_xt_10_floatF.json
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T double -f 1 -o xT_xt_10_doubleF.json
#diff xT_xt_10_floatF.json xT_xt_10_doubleF.json
