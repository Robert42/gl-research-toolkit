#version 450
#include <extensions/common.glsl>

#undef DEFERRED_RENDERER
#define FORWARD_RENDERER

#include "input-block.fs.glsl"
#include "forward-lighting.fs.glsl"

struct FramebufferTextures
{
  sampler2D worldNormal_normalLength;
  sampler2D meshNormal_meshBiTangentX;
  sampler2D meshTangent_meshBiTangentY;
  sampler2D meshBiTangentZ;
  sampler2D baseColor_metalMask;
  sampler2D emission_reflectance;
  sampler2D occlusion_smoothness;
  sampler2D zbuffer;
};

layout(binding=UNIFORM_BINDING_GBUFFER_BLOCK, std140) uniform FramebufferTextureBlock {
  FramebufferTextures textures;
};

in vec2 viewport_pos;
flat in mat4 inv_projection_view_matrix;


vec3 worldpos_from_ndc(mat4 inv_projection_view_matrix, vec3 view_ndc)
{
  // see also http://www.derschmale.com/2014/09/28/unprojections-explained/
  
  vec3 world_pos = transform_point(inv_projection_view_matrix, view_ndc);

  return world_pos;
}


vec3 worldpos_from_depth(mat4 inv_projection_view_matrix, vec2 viewport_pos, float depth)
{
  // Normalize to the Range [0, 1]
  depth = (depth - gl_DepthRange.near) / gl_DepthRange.diff;
  
  // map from [0, 1] to [-1, 1]
  float depth_ndc = depth*2-1;
  
  vec3 view_ndc = vec3(viewport_pos, depth_ndc);

  return worldpos_from_ndc(inv_projection_view_matrix, view_ndc);
}

vec3 worldpos_from_depth(float depth)
{
  return worldpos_from_depth(inv_projection_view_matrix, viewport_pos, depth);
}


void main()
{
  ivec2 texCoord = ivec2(gl_FragCoord.xy);

  float depth = texelFetch(textures.zbuffer, texCoord, 0).r;
  vec4 worldNormal_normalLength = texelFetch(textures.worldNormal_normalLength, texCoord, 0);
  vec4 meshNormal_meshBiTangentX = texelFetch(textures.meshNormal_meshBiTangentX, texCoord, 0);
  vec4 meshTangent_meshBiTangentY = texelFetch(textures.meshTangent_meshBiTangentY, texCoord, 0);
  float meshBiTangentZ = texelFetch(textures.meshBiTangentZ, texCoord, 0).x;
  vec4 baseColor_metalMask = texelFetch(textures.baseColor_metalMask, texCoord, 0);
  vec4 emission_reflectance = texelFetch(textures.emission_reflectance, texCoord, 0);
  vec2 occlusion_smoothness = texelFetch(textures.occlusion_smoothness, texCoord, 0).xy;

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
  gl_FragDepth = depth;
  surface.position = worldpos_from_depth(depth);
  
  vec3 meshNormal = normalize(meshNormal_meshBiTangentX.xyz);
  vec3 meshTangent = normalize(meshTangent_meshBiTangentY.xyz);
  vec3 meshBiTangent = normalize(vec3(meshNormal_meshBiTangentX.w, meshTangent_meshBiTangentY.w, meshBiTangentZ));

  float alpha = 1;
  mat3 tangent_to_worldspace = mat3(meshTangent, meshBiTangent, meshNormal);

  apply_material(material, surface, tangent_to_worldspace, alpha);
}
