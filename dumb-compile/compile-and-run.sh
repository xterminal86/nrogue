#!/bin/bash

display_usage() {
  echo "$0 <BUILD_TYPE> <CMAKE_JOBS=1> <USE_SDL=1> <SHOULD_RUN=0>";
  echo "<BUILD_TYPE>: \"Debug\" or \"Release\"";
}

if [ "$#" -eq 0 ]; then
  display_usage;
  exit 1;
fi

build_dir="build";
build_type=$1;
cmake_jobs=$2;
use_sdl=$3;
should_run=$4;

if [ -z "$cmake_jobs" ];
then
  cmake_jobs=1;
fi

if [ -z "$should_run" ];
then
  should_run=0;
fi

if [ -z "$use_sdl" ];
then
  use_sdl=1;
fi

if [ $build_type != "Debug" -a $build_type != "Release" ]; then
  echo "Wrong <BUILD_TYPE>: should be either \"Debug\" or \"Release\"";
  exit 1;
fi

if [ $use_sdl != "1" -a $use_sdl != "0" ];
then
  echo "<USE_SDL> should be 0 or 1";
  exit 1;
fi

if [ $use_sdl -eq 1 ]
then
  use_sdl="USE_SDL=ON";
else
  use_sdl="USE_SDL=OFF";
fi;

if [ ! -d $build_dir ]; then
  mkdir $build_dir
fi

cmake_command="cmake3";

if ! command -v $cmake_command &> /dev/null
then
  echo "No cmake found!";
  exit 1;
fi

rm -rf ${build_dir}

prepare="${cmake_command} -B ${build_dir} -D ${use_sdl} -D CMAKE_BUILD_TYPE=${build_type} ../";

echo ${prepare}

if [ $should_run -eq 1 ]
then
  echo "(will autorun after compiling)";
fi

eval ${prepare}

cmd="${cmake_command} --build ${build_dir} -j ${cmake_jobs}";

eval ${cmd}

if [ $should_run -ne 0 ]; then
  ./nrogue
fi

