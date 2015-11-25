#version 450 core

uniform MaterialInstance
{
  vec4 material_color;
} material;

out vec4 color;

void main()
{
  color = material.material_color;
}
