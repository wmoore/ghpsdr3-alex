#!/bin/bash
cd ..
./configure CFLAGS='-fopenmp -flto -O3 -march=native -I/usr/local/include/codec2' CXXFLAGS='-fopenmp -flto -O3 -march=native -I/usr/local/include/codec2'

