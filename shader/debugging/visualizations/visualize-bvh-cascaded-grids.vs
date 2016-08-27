#version 450 core

#include "common-debug-line-visualization.vs.glsl"

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform NodeBlock
{
  uint16_t grid_id;
};

void main()
{
  vec3 c = vec3(0);
  c[min(2, grid_id)] = 1;
  
  vec3 grid_position = vertex_position-0.5;
  
  vec3 ws_position = cascaded_grid_cell_to_worldspace(grid_position, grid_id);
  
  pass_attributes_to_fragment_shader(ws_position, c);
}
