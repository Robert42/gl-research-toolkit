#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif 
    writeonly uimage3D targetTextures[3];

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  uvec3 grid_cell = uvec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z%16);
  
  targetTextures[0] = scene.cascadedGrids.targetTexture0;
  targetTextures[1] = scene.cascadedGrids.targetTexture1;
  targetTextures[2] = scene.cascadedGrids.targetTexture2;
  
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif 
      writeonly uimage3D targetTexture = targetTextures[whichTexture];

  vec3 world_pos = cascaded_grid_cell_to_worldspace(grid_cell, whichTexture);
  
  
}
