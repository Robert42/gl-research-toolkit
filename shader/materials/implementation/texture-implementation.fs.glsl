
#include "input-block.fs.glsl"
#include "material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>
  
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
