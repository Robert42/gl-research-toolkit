
#include "input-block.fs.glsl"
#include "material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>
  
layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK) uniform MaterialInstanceBlock
{
  // sampler2D within Uniform Block possible thanks to GL_NV_bindless_texture
  sampler2D diffuse_map;
  sampler2D normal_map;
  sampler2D rhmo_map;
  sampler2D emission_map;
  vec4 tint;
  vec2 roughness_range;
  vec2 metallicness_range;
  vec2 occlusion_range;
  float emission_factor;
}material_instance;


MaterialOutput calculate_material_output()
{
  MaterialOutput material_output;
  
  vec2 uv = fragment.uv; // TODO simple parallax mapping using the green channel from rhmo_map
  
  vec4 rhmo = texture2D(material_instance.rhmo_map, uv);
  
  float roughness = rhmo[0];
  float metalicness = rhmo[2];
  float occlusion = rhmo[3];
  
  material_output.color = texture2D(material_instance.diffuse_map, uv) * material_instance.tint;
  material_output.emission = texture2D(material_instance.emission_map, uv).xyz * material_instance.emission_factor;
  material_output.roughness = mix(roughness, material_instance.roughness_range[0], material_instance.roughness_range[1]);
  material_output.metallicness = mix(metalicness, material_instance.metallicness_range[0], material_instance.metallicness_range[1]);
  material_output.occlusion = mix(occlusion, material_instance.occlusion_range[0], material_instance.occlusion_range[1]);
  material_output.position = fragment.position;
  material_output.normal = fragment.normal; // TODO implement normal mapping
  
  return material_output;
}
