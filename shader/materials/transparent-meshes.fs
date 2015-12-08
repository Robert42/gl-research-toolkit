#version 450 core
#extension GL_NV_bindless_texture : require

#include "implementation/texture-implementation.fs.glsl"

void main()
{
  MaterialOutput material_output = calculate_material_output();
  
  apply_material(material_output, direction_to_camera());
}
