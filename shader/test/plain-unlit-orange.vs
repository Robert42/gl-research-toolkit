#version 450 core

#include "../scene/transformation.glsl"


uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
} u;



in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec2 uv;

void main()
{
  gl_Position = u.view_projection * u.model_matrix * vec4(position.xyz, 1);
}
