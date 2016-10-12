#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform NodeBlock
{
  uint16_t grid_id;
};

void main()
{
  vec3 grid_position = vertex_position;
  
  vec3 gridLocationOffset = scene.candidateGridHeader.gridLocation.xyz;
  float gridLocationScale = scene.candidateGridHeader.gridLocation.w;
  
  vec3 ws_position = (vertex_position - gridLocationOffset)/gridLocationScale;
  
  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
