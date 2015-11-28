#version 450 core

#include <debugging/normal.glsl>

uniform TestUniformBlock
{
  mat4 model_matrix;
  mat4 view_projection;
  vec4 material_color;
  int debuggingMode;
} u;

layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_normal;
layout(location=2) in vec3 vertex_tangent;
layout(location=3) in vec2 vertex_uv;

out vec3 vertex_color;

#include "debugging.glsl"

void main()
{
  switch(u.debuggingMode)
  {
  case DebuggingNormals:
    vertex_color = encode_direction_as_color(vertex_normal);
    break;
  case DebuggingUV:
    vertex_color = vec3(vertex_uv, 0);
    break;
  case DebuggingNone:
  case DebuggingPlainColor:
  default:
    vertex_color = u.material_color.rgb;
  }
  gl_Position = u.view_projection * u.model_matrix * vec4(vertex_position.xyz, 1);
}
