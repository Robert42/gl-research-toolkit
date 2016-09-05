#! /bin/bash

export FILE=CMakeLists.txt

echo -e "cmake_minimum_required(VERSION 3.1 FATAL_ERROR)\n\nadd_custom_target(\n  gl-research-toolkit-shader\n  SOURCES" > $FILE

find . | grep -E '\.(cs|vs|fs|glsl|h)$' | sort -d | sed -E 's/^\.\//  /g' >> $FILE

echo ")" >> $FILE

