#!/bin/bash

set -u

inc=$(find ../src/ -type d)

inc_dirs=""

for dir in ${inc}
do
  inc_dirs="$inc_dirs -I $dir"
done

fname="cppcheck-result.txt"

echo "Starting cppcheck with output to ${fname}, please wait..."

cppcheck --enable=warning,performance,information,unusedFunction $inc_dirs ../src/ &> $fname

echo "cppcheck has finished - $fname written"

