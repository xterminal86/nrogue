#!/bin/bash

find src/ -type f \( -iname \*.cpp -o -iname \*.h \) | xargs wc -l
