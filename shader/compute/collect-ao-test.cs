#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>
#include "ao-collect-uniform.glsl"

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

void main()
{
  get_cascaded_grid_images(0);
  
  if(gl_GlobalInvocationID.z < 16)
  {
    imageStore(leafIndexTexture, ivec3(gl_GlobalInvocationID), uvec4(255, 0, 255, 0));
  #if BVH_USE_GRID_OCCLUSION
    imageStore(occlusionTexture, ivec3(gl_GlobalInvocationID), vec4(vec3(gl_GlobalInvocationID)/16., 1));
  #endif
  }
}
