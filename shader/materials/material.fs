#version 450 core
#extension GL_NV_bindless_texture : require
#extension GL_NV_gpu_shader5 : require
#extension GL_NV_command_list : require

layout(commandBindableNV)uniform;

#ifdef DEPTH_PREPASS
#ifndef MASKED
layout(early_fragment_tests) in;
#endif
#endif

#include "implementation/input-block.fs.glsl"
#include "implementation/material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>


#ifdef PLAIN_COLOR
// TODO: improve performance by doing this in the vertex shader and just passing the MaterialOutput instance (flat, without interpolation)
layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
  vec3 base_color;
  float smoothness;
  vec3 emission;
  float metal_mask;
}material_instance;

void main()
{
#ifdef DEPTH_PREPASS
  fragment_color = vec4(1, 0, 1, 1);
  return;
#endif
  BaseMaterial material;
  
  material.normal = fragment.normal;
  material.smoothness = material_instance.smoothness;
  material.base_color = material_instance.base_color;
  material.metal_mask  = material_instance.metal_mask;
  material.emission = material_instance.emission;
  material.reflectance = 0.5f;
  material.occlusion = 1;
  
  SurfaceData surface;
  surface.position = fragment.position;
  
  // No normal mapping here, so uv and tangent are unused
  
  apply_material(material, surface, 1.f);
}
#else

layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
vec4 tint;
vec4 srmo_range_0;
vec4 srmo_range_1;
float emission_factor;
// sampler2D within Uniform Block possible thanks to GL_NV_bindless_texture
sampler2D basecolor_map;
sampler2D normal_map;
sampler2D height_map;
sampler2D srmo_map; // smoothness, reflectance, metalic_map, occlusion
sampler2D emission_map;
}material_instance;



void calculate_material_output(out BaseMaterial material, out SurfaceData surface, out float alpha)
{
  vec2 uv = fragment.uv;
  
  // TODO simple parallax mapping using height_map
  
  vec4 color = texture2D(material_instance.basecolor_map, uv) * material_instance.tint;
  
#ifndef DEPTH_PREPASS
  vec4 srmo = texture2D(material_instance.srmo_map, uv);
  
  srmo = mix(material_instance.srmo_range_0, material_instance.srmo_range_1, srmo);
  
  float smoothness = srmo[0];
  float reflectance = srmo[1];
  float metal_mask = srmo[2];
  float occlusion = srmo[3];
  
  material.normal = fragment.normal; // TODO implement normal mapping
  material.smoothness = smoothness;
  material.base_color = color.rgb;
  material.metal_mask = metal_mask;
  material.emission = texture2D(material_instance.emission_map, uv).xyz * material_instance.emission_factor;
  material.reflectance = reflectance;
  material.occlusion = occlusion;
  
  surface.position = fragment.position;
  
  #ifdef TWO_SIDED
  material.normal.z = mix(-material.normal.z, material.normal.z, gl_FrontFacing);
  #endif
#endif
  alpha = color.a;
}


void main()
{
#ifdef DEPTH_PREPASS
  fragment_color = vec4(1, 0, 1, 1);
#ifdef OPAQUE
  return;
#endif
#endif

  BaseMaterial material;
  SurfaceData surface;
  float alpha;
  calculate_material_output(material, surface, alpha);
  
#ifdef DEPTH_PREPASS
#ifdef MASKED
  if(alpha < MASK_THRESHOLD)
    discard;
#endif
  return;
#endif
  
  apply_material(material, surface, alpha);
}

#endif
