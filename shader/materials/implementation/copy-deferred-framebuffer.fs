#version 450
#include <extensions/common.glsl>

#undef DEFERRED_RENDERER
#define FORWARD_RENDERER

struct FramebufferTextures
{
  sampler2D worldNormal_normalLength;
  sampler2D baseColor_metalMask;
  sampler2D emission_reflectance;
  sampler2D occlusion_smoothness;
};

layout(binding=0, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

out vec4 color;

void main()
{
  // vec3 world_pos = ; TODO!!!!

  color = texelFetch(textures.baseColor_metalMask, ivec2(gl_FragCoord.xy), 0);
}

