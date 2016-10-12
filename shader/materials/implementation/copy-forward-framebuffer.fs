#version 450
#include <extensions/common.glsl>

struct FramebufferTextures
{
  sampler2D color;
  sampler2D depth;
};

layout(binding=0, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

out vec4 color;

void main()
{
  ivec2 texCoord = ivec2(gl_FragCoord.xy);
  
  color = texelFetch(textures.color, texCoord, 0);
  gl_FragDepth = texelFetch(textures.depth, texCoord, 0).r;
}
