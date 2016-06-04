#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform SphereBlock
{
  vec3 origin;
  float radius;
};


void main()
{
  vec3 ws_position = vertex_position * radius + origin;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
