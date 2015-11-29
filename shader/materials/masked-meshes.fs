#version 450 core

#include "implementation/texture-implementation.fs.glsl"

void main()
{
  MaterialOutput material_output = calculate_material_output();
  
  if(material_output.color.a < 0.6f)
    discard;
  
  apply_material(material_output);
}
