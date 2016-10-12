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
  
  vec3 gridLocationOffset = scene.candidateGridHeader.fallbackSdfGridLocation.xyz;
  float gridLocationScale = scene.candidateGridHeader.fallbackSdfGridLocation.w;
  
  int dimension = int(vertex_parameter1);
  int index = int(vertex_parameter2);
  
  ivec3 gridSize = textureSize(scene.candidateGridHeader.fallbackSDF, 0);

  grid_position *= gridSize;
  grid_position[dimension] = index;

  vec3 ws_position = (grid_position - gridLocationOffset)/gridLocationScale;
  
  if(index > gridSize[dimension])
    ws_position = vec3(0);
  
  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
