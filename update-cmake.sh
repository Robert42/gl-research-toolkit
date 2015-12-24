#! /bin/bash

cd ./external/angelscript-integration/integration
./update-cmake.sh
cd - > /dev/null

cd ./external/shader
./update-cmake.sh
cd - > /dev/null

cd ./gl-research-toolkit
./update-cmake.sh
cd - > /dev/null

cd ./shader
./update-cmake.sh
cd - > /dev/null
