#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct IblHeader
{
  layout(r16f) writeonly image2D targetTexture;
  sampler2D sourceTexture;
  mat4 cube_side;
};

layout(binding=IBL_BLOCK, std140)
uniform IblHeaderBlock
{
  IblHeader header;
};

void main()
{
  ivec2 texture_coordinate = ivec2(gl_GlobalInvocationID.xy);
  imageStore(header.targetTexture, texture_coordinate, vec4(1.0, 0.5, 0.0, 1.0));
}
