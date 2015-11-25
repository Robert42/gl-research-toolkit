#version 450 core

#include "../scene/transformation.glsl"


uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
} u;



layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec2 uv;

void main()
{
  gl_Position = u.view_projection * u.model_matrix * vec4(position.xyz, 1);
}
