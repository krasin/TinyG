#!/bin/bash

# TinyG simulator tests. They don't require any hardware, 
# but also don't guarantee that the simulation is precise.

set -o nounset
set -o errexit

echo "============================================="
echo "Running very basic test with full checks ..."
echo "============================================="
echo ""

echo -n "test.gcode -- "
( cat test.gcode | grep -v "^;" | ./tinyg.elf 2>&1 | FileCheck test.gcode && echo "OK" ) || echo "FAIL"


echo ""
echo "============================================="
echo "Running full gcode test suite ..."
echo "Note: currently, it only tests that the simulator exit code."
echo "============================================="

for i in ../../../gcode_samples/*.gcode
do
  echo -n "$i -- "
  OUT_FILE=`basename $i`.out
  (( ./tinyg.elf $i > $OUT_FILE 2>&1 ) && echo "OK" ) || ( echo "FAIL" && cat $OUT_FILE )
done
