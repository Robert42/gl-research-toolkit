#version 450 core

#include "common-debug-line-visualization.vs.glsl"

#include <lighting/light-structs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform DirectionBlock
{
  vec3 from;
  vec3 to;
};


void main()
{
  vec3 z = normalize(to-from);
  vec3 x;
  vec3 y;
  
  if(abs(dot(z, vec3(0,1,0))) < abs(dot(z, vec3(0, 0, 1))))
    x = cross(vec3(0, 1, 0), z);
  else
    x = cross(vec3(0, 0, 1), z);
  
  x = normalize(x);
  y = cross(x, z);
  
  mat4 t;
  t[0] = vec4(x, 0);
  t[1] = vec4(y, 0);
  t[2] = vec4(z, 0);
  t[3] = vec4(from, 1);
  
  vec3 ws_position = (t * vec4(vertex_position, 1)).xyz;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
