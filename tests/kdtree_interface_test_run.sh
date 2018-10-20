#!/bin/bash
P=.
$P/nanoflann_any_test list
DIM="${DIM:-10}"
KNN=12
T1="${T1:-float}"
T2="${T2:-double}"
TREEPTS=1000
TESTPTS=10
#file is of floats
[ ! -f xTr.bin ] && $P/nanoflann_any_test gentrain -d $DIM --count $TREEPTS --minrange -1000.0 --maxrange 1000.0 -i xTr.bin
[ ! -f xte.bin ] && $P/nanoflann_any_test gentest -d $DIM --count $TESTPTS --minrange -1000.0 --maxrange 1000.0 -t xTe.bin
$P/nanoflann_any_test dumpfile -i xTe.bin -d $DIM > xTe.bin.txt
$P/nanoflann_any_test dumpfile -i xTr.bin -d $DIM > xTr.bin.txt
# using build from flosting tree
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d 10 -k 10 -T float -f 1 -o xT_xt_10_float_float.json
# using build from flosting ... not
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T $T1 -o xT_xt_${DIM}_${T1}.json
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T $T2 -o xT_xt_${DIM}_${T2}.json
diff xT_xt_${DIM}_${T1}.json xT_xt_${DIM}_${T2}.json

$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T $T1 -f 1 -o xT_xt_${DIM}_${T1}_F.json
$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T $T2 -f 1 -o xT_xt_${DIM}_${T2}_F.json
diff xT_xt_${DIM}_${T1}_F.json xT_xt_${DIM}_${T2}_F.json
diff xT_xt_${DIM}_${T1}.json xT_xt_${DIM}_${T2}_F.json

#TODO FIXME
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T float -f 1 -o xT_xt_10_floatF.json
#$P/nanoflann_any_test run -i xTr.bin -t xTe.bin -d $DIM -k $KNN -T double -f 1 -o xT_xt_10_doubleF.json
#diff xT_xt_10_floatF.json xT_xt_10_doubleF.json
stty sane
