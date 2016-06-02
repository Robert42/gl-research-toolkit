#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform ConeBlock
{
  vec3 origin;
  float tan_half_angle;
  vec3 direction;
  float _padding;
};



void main()
{
  mat4 t = matrixForDirection(direction, origin);
  
  vec3 v = vertex_position;
  
  v.xy *= v.z * tan_half_angle;
  
  vec3 ws_position = (t * vec4(v, 1)).xyz;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
