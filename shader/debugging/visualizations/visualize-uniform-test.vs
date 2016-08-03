#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

void main()
{
  vec3 ws_position = vertex_position;

  pass_attributes_to_fragment_shader_no_transform(ws_position, vertex_color);
}
