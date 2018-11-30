#!/bin/bash

inc=$(find inc/ -type d);

inc_dirs=""

for dir in ${inc}
do
  inc_dirs="$inc_dirs -I $dir";
done

fname="cppcheck-result.txt";

cppcheck $inc_dirs src/ --enable=all &> $fname

echo "cppcheck has finished - $fname written";
