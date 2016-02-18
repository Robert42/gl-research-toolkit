#version 450 core
#extension GL_NV_bindless_texture : require

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
vec2 smoothness_range;
vec2 occlusion_range;
vec2 reflectance_range;
float emission_factor;
// sampler2D within Uniform Block possible thanks to GL_NV_bindless_texture
sampler2D diffuse_map;
sampler2D normal_map;
sampler2D height_map;
sampler2D srmo_map; // smoothness, reflectance, metalic_map, occlusion
sampler2D emission_map;
}material_instance;



void calculate_material_output(out BaseMaterial material, out SurfaceData surface, out float alpha)
{
  vec2 uv = fragment.uv; // TODO simple parallax mapping using the green channel from rhmo_map
  
  vec4 srmo = texture2D(material_instance.srmo_map, uv);
  
  float smoothness = srmo[0];
  float reflectance = srmo[1];
  float metal_mask = srmo[2];
  float occlusion = srmo[3];
  
  vec4 color = texture2D(material_instance.diffuse_map, uv) * material_instance.tint;
  
  material.normal = fragment.normal; // TODO implement normal mapping
  material.smoothness = mix(material_instance.smoothness_range[0], material_instance.smoothness_range[1], smoothness);
  material.base_color = color.rgb;
  material.metal_mask = metal_mask;
  material.emission = texture2D(material_instance.emission_map, uv).xyz * material_instance.emission_factor;
  material.reflectance = mix(material_instance.reflectance_range[0], material_instance.reflectance_range[1], reflectance);
  material.occlusion = mix(material_instance.occlusion_range[0], material_instance.occlusion_range[1], occlusion);
  
  surface.position = fragment.position;
  
  alpha = color.a;
}


void main()
{
  BaseMaterial material;
  SurfaceData surface;
  float alpha;
  calculate_material_output(material, surface, alpha);
  
  apply_material(material, surface, alpha);
}

#endif
