#version 450 core

#include <debugging/normal.glsl>

layout(location=0) in vec3 vertex_position;
layout(location=1) in vec3 vertex_normal;
layout(location=2) in vec3 vertex_tangent;
layout(location=3) in vec2 vertex_uv;

out vec3 fragment_color;
out vec3 fragment_normal;

#include "debugging.glsl"

void main()
{
  
  fragment_normal = (u.model_matrix * vec4(vertex_normal, 0)).xyz;
  gl_Position = u.view_projection * u.model_matrix * vec4(vertex_position.xyz, 1);
    
  switch(u.debuggingMode)
  {
  case DebuggingNormals:
    fragment_color = encode_direction_as_color(vertex_normal);
    break;
  case DebuggingTangents:
    fragment_color = encode_direction_as_color(vertex_tangent);
    break;
  case DebuggingUV:
    fragment_color = vec3(vertex_uv, 0);
    break;
  case ShowUV:
    gl_Position = vec4(mix(vec2(-1), vec2(1), vertex_uv), 0, 1);
  case DebuggingNone:
  case DebuggingPlainColor:
  default:
    fragment_color = u.material_color.rgb;
  }

}
