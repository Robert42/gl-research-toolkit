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

#if defined(TEXTURE_BASECOLOR) || defined(TEXTURE_BASECOLOR_ALPHA) || defined(TEXTURE_NORMAL_LS) || defined(TEXTURE_HEIGHT) || defined(TEXTURE_SMOOTHENESS) || defined(TEXTURE_REFLECTIVITY) || defined(TEXTURE_METALLIC) || defined(TEXTURE_AO) || defined(TEXTURE_EMISSION)
  fragment_color = checkerboard();
  return;
#endif

  BaseMaterial material;
  
  material.normal = normalize(fragment.normal);
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
  
  float height = texture2D(material_instance.height_map, uv).r;
  // TODO simple parallax mapping using height_map
  
  vec4 color = texture2D(material_instance.basecolor_map, uv) * material_instance.tint;
#ifndef DEPTH_PREPASS
  vec3 normal = texture2D(material_instance.normal_map, uv).xyz;
  vec4 srmo = texture2D(material_instance.srmo_map, uv);
  vec3 emission = texture2D(material_instance.emission_map, uv).rgb;

  #if defined(TEXTURE_BASECOLOR)
    fragment_color = color;
    return;
  #elif defined(TEXTURE_BASECOLOR_ALPHA)
    fragment_color = vec4(color.aaa, 1);
    return;
  #elif defined(TEXTURE_NORMAL_LS)
    fragment_color = vec4(encode_direction_as_color(normal), 1);
    return;
  #elif defined(TEXTURE_HEIGHT)
    fragment_color = vec4(vec3(height), 1);
    return;
  #elif defined(TEXTURE_SMOOTHENESS)
    fragment_color = vec4(vec3(srmo[0]), 1);
    return;
  #elif defined(TEXTURE_REFLECTIVITY)
    fragment_color = vec4(vec3(srmo[1]), 1);
    return;
  #elif defined(TEXTURE_METALLIC)
    fragment_color = vec4(vec3(srmo[2]), 1);
    return;
  #elif defined(TEXTURE_AO)
    fragment_color = vec4(vec3(srmo[3]), 1);
    return;
  #elif defined(TEXTURE_EMISSION)
    fragment_color = vec4(vec3(emission), 1);
    return;
  #endif
  
  vec3 fragment_normal = normalize(fragment.normal);
  vec3 fragment_tangent = normalize(fragment.tangent);
  vec3 fragment_bitangent = normalize(fragment.bitangent);
  normal = normalize(normal.x * fragment_tangent + normal.y * fragment_bitangent + normal.z * fragment_normal);
  
  srmo = mix(material_instance.srmo_range_0, material_instance.srmo_range_1, srmo);
  
  float smoothness = srmo[0];
  float reflectance = srmo[1];
  float metal_mask = srmo[2];
  float occlusion = srmo[3];
  
  emission *= material_instance.emission_factor;
  
  material.normal = normal;
  material.smoothness = smoothness;
  material.base_color = color.rgb;
  material.metal_mask = metal_mask;
  material.emission = emission;
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
  // ignore the alpha in the depth prepass, if the material is opaque, or the alpha channel itself is current being debugged
#if defined(OPAQUE) || defined(MATERIAL_NORMAL_WS) || defined(MATERIAL_ALPHA)
  return;
#endif
#endif

  BaseMaterial material;
  SurfaceData surface;
  float alpha;
  calculate_material_output(material, surface, alpha);
  
#if defined(TEXTURE_BASECOLOR) || defined(TEXTURE_BASECOLOR_ALPHA) || defined(TEXTURE_NORMAL_LS) || defined(TEXTURE_HEIGHT) || defined(TEXTURE_SMOOTHENESS) || defined(TEXTURE_REFLECTIVITY) || defined(TEXTURE_METALLIC) || defined(TEXTURE_AO) || defined(TEXTURE_EMISSION)
  return;
#endif
  
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
