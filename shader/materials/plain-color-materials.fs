#version 450 core

#include "implementation/input-block.fs.glsl"
#include "implementation/material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
  vec4 diffuse_color_and_roughness;
  vec4 emission_and_metallic;
}material_instance;

void main()
{
  MaterialOutput material_output;
  
  vec3 diffuse = material_instance.diffuse_color_and_roughness.rgb;
  float roughness = material_instance.diffuse_color_and_roughness.a;
  vec3 emission = material_instance.emission_and_metallic.rgb;
  float metallic = material_instance.emission_and_metallic.a;
  
  material_output.color = vec4(diffuse, 1);
  material_output.emission = vec3(emission);
  material_output.roughness = roughness;
  material_output.position = fragment.position;
  material_output.metallic = metallic;
  material_output.normal = fragment.normal;
  material_output.occlusion = 0;
  
  // No normal mapping here, so uv and tangent ar eunused
  
  apply_material(material_output);
}
