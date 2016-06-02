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
  mat4 t = matrixForDirection(normalize(to-from), from);
  
  vec3 ws_position = (t * vec4(vertex_position, 1)).xyz;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
