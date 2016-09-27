#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform BoundingSphere
{
  vec3 origin;
  float radius;
};

const float is_grid = vertex_parameter1;
const float is_using_extended_radius = vertex_parameter2;

vec3 grid_position()
{
  vec3 gridLocationOffset = scene.candidateGridHeader.gridLocation.xyz;
  float gridLocationScale = scene.candidateGridHeader.gridLocation.w;

  return (vertex_position - gridLocationOffset)/gridLocationScale;
}

vec3 sphere_position()
{
  float r = radius + AO_RADIUS * is_using_extended_radius;
  
  return vertex_position * r + origin;
}

void main()
{
  vec3 ws_position = mix(sphere_position(), grid_position(), is_grid);
  
  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
