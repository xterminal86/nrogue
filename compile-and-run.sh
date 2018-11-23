#!/bin/bash

sources=$(find src/ -iname "*.cpp");
inc_ls=$(find inc/ -mindepth 1 -maxdepth 1 -type d)
total_inc="";
for d in ${inc_ls[@]};
do
  total_inc+="-iquote $d ";
done
g++ -O3 -Wall -mcmodel=medium $sources $total_inc -o nrogue -lcurses && ./nrogue
