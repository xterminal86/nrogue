#!/bin/bash

cd build/
rm -rf *
cmake3 ../
make -j4
./nrogue

