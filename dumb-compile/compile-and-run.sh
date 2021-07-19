#!/bin/bash

display_usage() {
  echo "$0 <BUILD_TYPE> <CMAKE_JOBS=1> <SHOULD_RUN=0>";
  echo "<BUILD_TYPE>: \"Debug\" or \"Release\"";
}

if [ "$#" -eq 0 ]; then
  display_usage;
  exit 1;
fi

build_dir="build";
build_type=$1;
cmake_jobs=$2;
should_run=$3;

if [ -z "$cmake_jobs" ]; then
  cmake_jobs=1;
fi

if [ -z "$should_run" ]; then
  should_run=0;
fi

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

cmake_command="cmake";

if ! command -v $cmake_command &> /dev/null
then
  cmake_command="cmake3";
fi

if ! command -v $cmake_command &> /dev/null
then
  echo "No cmake or cmake3 found!";
  exit 1;
fi

rm -rf *
$cmake_command -D USE_SDL=ON -D CMAKE_BUILD_TYPE=$build_type ../../ && make -j$cmake_jobs

if [ $should_run -ne 0 ]; then
  ./nrogue
fi

