#version 450 core

#include "implementation/input-block.fs.glsl"
#include "implementation/material-implementation.fs.glsl"

#include <glrt/glsl/layout-constants.h>


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
