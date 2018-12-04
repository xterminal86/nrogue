#!/bin/bash

build_dir="build";

if [ ! -d $build_dir ]; then
  mkdir $build_dir
fi

cd $build_dir

if [ $? -ne 0 ]; then
  echo "Cannot cd into build directory!"
  exit 1;
fi

rm -rf *
cmake -D CMAKE_BUILD_TYPE=Release ../../ || cmake3 -D CMAKE_BUILD_TYPE=Release ../../
make -j4
./nrogue

