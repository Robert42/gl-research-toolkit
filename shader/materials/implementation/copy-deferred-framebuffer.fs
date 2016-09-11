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
  sampler2D zbuffer;
};

layout(binding=UNIFORM_BINDING_GBUFFER_BLOCK, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

in vec2 viewport_pos;

void main()
{
  ivec2 texCoord = ivec2(gl_FragCoord.xy);

  float depth = texelFetch(textures.zbuffer, texCoord, 0).r;
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


  SurfaceData surface;
  surface.position = worldpos_from_depth(depth);

  float alpha = 1;
  mat3 tangent_to_worldspace = matrix3x3ForDirection(material.normal);

  apply_material(material, surface, tangent_to_worldspace, alpha);
}

