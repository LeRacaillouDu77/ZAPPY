#!/bin/bash
mkdir -p build && cd build
cmake ..
make
cd ..
chmod +x zappy_ai