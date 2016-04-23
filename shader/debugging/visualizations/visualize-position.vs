#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform PositionBlock
{
  vec3 position;
};


void main()
{
  vec3 ws_position = vertex_position + position;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
