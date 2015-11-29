#! /bin/bash

echo -e "cmake_minimum_required(VERSION 3.1 FATAL_ERROR)\n\nadd_custom_target(\n  gl-research-toolkit-shader\n  SOURCES" > CMakeLists.txt

find . | grep -E '\.(vs|fs|glsl)$' | sort | sed -E 's/^\.\//  /g' >> CMakeLists.txt

echo ")" >> CMakeLists.txt

