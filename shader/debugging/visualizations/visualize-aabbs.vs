#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform AabbBlock
{
  vec3 minVertex;
  float _padding1;
  vec3 maxVertex;
  float _padding2;
};


void main()
{
  vec3 ws_position = mix(minVertex, maxVertex, vertex_position);

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
