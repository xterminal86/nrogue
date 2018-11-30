#!/bin/bash

build_dir="build";

if [ ! -d $build_dir ]; then
  mkdir $build_dir
fi

cd $build_dir
rm -rf *
cmake -D CMAKE_BUILD_TYPE=Release ../../ || cmake3 -D CMAKE_BUILD_TYPE=Release ../../
make -j4
./nrogue

