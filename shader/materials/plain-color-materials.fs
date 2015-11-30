#version 450 core

#include "implementation/input-block.fs.glsl"
#include "implementation/material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK) uniform MaterialInstanceBlock
{
  vec3 diffuse;
  float roughness;
  float metallicness;
}material_instance;

void main()
{
  MaterialOutput material_output;
  
  material_output.color = vec4(material_instance.diffuse, 1);
  material_output.roughness = material_instance.roughness;
  material_output.position = fragment.position;
  material_output.metallicness = material_instance.metallicness;
  material_output.normal = fragment.normal;
  material_output.occlusion = 0;
  
  // No normal mapping here, so uv and tangent ar eunused
  
  apply_material(material_output);
}
