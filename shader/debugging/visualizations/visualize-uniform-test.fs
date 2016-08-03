#version 450 core

#include "common-debug-line-visualization.fs.glsl"

void main()
{
  apply_color(DEBUG_FRAGMENT_COORD(ORANGE_COLOR));
}
