#version 450 core

uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
} u;

out vec4 color;

void main()
{
  color = u.material_color;
}
