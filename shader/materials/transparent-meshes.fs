#version 450 core
#extension GL_NV_bindless_texture : require

#include "implementation/texture-implementation.fs.glsl"

void main()
{
  BaseMaterial material;
  SurfaceData surface;
  float alpha;
  calculate_material_output(material, surface, alpha);
  
  apply_material(material, surface, alpha);
}
