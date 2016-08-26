#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  uvec3 grid_cell = uvec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z%16);
  
#ifdef IMAGE_FORMAT_R16UI
  layout(r16ui)
#else
  layout(rgba16ui)
#endif 
      writeonly uimage3D targetTexture = scene.cascadedGrids.targetTexture[whichTexture];
  vec3 world_pos = cascaded_grid_cell_to_worldspace(grid_cell, whichTexture);
}