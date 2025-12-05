#!/bin/bash

cd build
rm -rf vad_test
cmake ..
make
cd ..

./build/vad_test test_wav.wav
