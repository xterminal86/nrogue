#!/bin/bash

if [ "$#" -eq 0 ]; then
  echo "$0 <BUILD_TYPE>";
  echo "<BUILD_TYPE>: \"Debug\" or \"Release\"";
  exit 1;
fi

build_dir="build";
build_type=$1;

if [ $build_type != "Debug" -a $build_type != "Release" ]; then
  echo "Wrong <BUILD_TYPE>: should be either \"Debug\" or \"Release\"";
  exit 1;
fi

if [ ! -d $build_dir ]; then
  mkdir $build_dir
fi

cd $build_dir

if [ $? -ne 0 ]; then
  echo "Cannot cd into build directory!"
  exit 1;
fi

rm -rf *
cmake -D USE_SDL=ON -D CMAKE_BUILD_TYPE=$build_type ../../ || cmake3 -D USE_SDL=ON -D CMAKE_BUILD_TYPE=$build_type ../../
make -j4
./nrogue

