#!/bin/bash
 (make && ./nanoflann_kdd | tee b )&& (../scripts/nanoflann_kdd_analyze.py  b)
