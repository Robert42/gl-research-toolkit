#version 450 core

#include "../scene/transformation.glsl"


uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
} u;



in vec2 position;

void main()
{
  gl_Position = u.view_projection * u.model_matrix * vec4(position.x, position.y, 0, 1);
}
