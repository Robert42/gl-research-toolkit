#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>
#include "ao-collect-uniform.glsl"

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

void main()
{
  uint texture_index = gl_GlobalInvocationID.z / 16;
  ivec3 grid_cell = ivec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z%16);
    
  get_cascaded_grid_images(texture_index);
  
  imageStore(leafIndexTexture, grid_cell, uvec4(grid_cell*16, texture_index));
#if BVH_USE_GRID_OCCLUSION
  imageStore(occlusionTexture, grid_cell, vec4(vec3(grid_cell)/16., 1));
#endif
}
