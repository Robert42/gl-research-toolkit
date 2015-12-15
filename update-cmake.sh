#! /bin/bash

cd ./external/shader
./update-cmake.sh
cd - > /dev/null

cd ./gl-research-toolkit
./update-cmake.sh
cd - > /dev/null

cd ./shader
./update-cmake.sh
cd - > /dev/null
