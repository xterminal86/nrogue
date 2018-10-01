#!/bin/bash

g++ -Wall -mcmodel=medium src/*.cpp -I./inc -o nrogue -lcurses && ./nrogue
