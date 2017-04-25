#!/bin/bash

mkdir -p build
cd build
cmake ..
make parse_program_to_dot
