#! /bin/bash

cd ./external/angelscript-integration/integration
./update-cmake.sh
cd - > /dev/null

cd ./external/shader
./update-cmake.sh
cd - > /dev/null

cd ./gl-research-toolkit/core
./update-cmake.sh
cd - > /dev/null

cd ./gl-research-toolkit/scene
./update-cmake.sh
cd - > /dev/null

cd ./gl-research-toolkit/renderer
./update-cmake.sh
cd - > /dev/null

cd ./shader
./update-cmake.sh
cd - > /dev/null

cd ./tests
./update-cmake.sh
cd - > /dev/null
