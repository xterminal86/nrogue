#!/bin/bash

build_dir="build";

if [ ! -d $build_dir ]; then
  mkdir $build_dir
fi

cd $build_dir
rm -rf *
cmake3 ../../
make -j4
./nrogue

