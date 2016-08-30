#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct TestHeader
{
  layout(r8) image3D targetTexture;
};

layout(binding=0, std140)
uniform TestBlock
{
  TestHeader header;
};


void main()
{
  if(gl_GlobalInvocationID.z < 16)
    imageStore(header.targetTexture, ivec3(gl_GlobalInvocationID), vec4(vec3(gl_GlobalInvocationID)/16., 1));
}
