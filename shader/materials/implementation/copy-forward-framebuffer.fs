#version 450
#include <extensions/common.glsl>

struct FramebufferTextures
{
  sampler2D color;
};

layout(binding=0, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

out vec4 color;

void main()
{
  color = texelFetch(textures.color, ivec2(gl_FragCoord.xy), 0);
}

