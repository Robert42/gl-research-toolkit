#version 450
#include <extensions/common.glsl>

#undef DEFERRED_RENDERER
#define FORWARD_RENDERER

#define N_GI_CONES 9
#include "input-block.fs.glsl"
#include "forward-lighting.fs.glsl"

struct FramebufferTextures
{
  sampler2D worldNormal_normalLength;
  sampler2D baseColor_metalMask;
  sampler2D emission_reflectance;
  sampler2D occlusion_smoothness;
};

layout(binding=UNIFORM_BINDING_GBUFFER_BLOCK, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

out vec4 color;

void main()
{
  ivec2 texCoord = ivec2(gl_FragCoord.xy);

  SurfaceData surface;
//   surface.world_pos = ; TODO!!!!

  vec4 worldNormal_normalLength = texelFetch(textures.worldNormal_normalLength, texCoord, 0);
  vec4 baseColor_metalMask = texelFetch(textures.baseColor_metalMask, texCoord, 0);
  vec4 emission_reflectance = texelFetch(textures.emission_reflectance, texCoord, 0);
  vec4 occlusion_smoothness = texelFetch(textures.occlusion_smoothness, texCoord, 0);

  BaseMaterial material;
  material.normal = worldNormal_normalLength.xyz;
  material.normal_length = worldNormal_normalLength.w;
  material.base_color = baseColor_metalMask.xyz;
  material.metal_mask = baseColor_metalMask.w;
  material.emission = emission_reflectance.xyz;
  material.reflectance = emission_reflectance.w;
  material.occlusion = occlusion_smoothness[0];
  material.smoothness = occlusion_smoothness[1];

  color = vec4(material.base_color, 1);
}

