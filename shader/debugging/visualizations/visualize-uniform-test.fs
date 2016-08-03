#version 450 core

#include "common-debug-line-visualization.fs.glsl"

void main()
{
  PRINT_VALUE(42);
  apply_color();
}
