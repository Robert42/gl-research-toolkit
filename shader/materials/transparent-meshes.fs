#version 450 core

#include "implementation/texture-implementation.fs.glsl"

void main()
{
  MaterialOutput material_output = calculate_material_output();
  
  apply_material(material_output);
}
