#include "output-block.vs.glsl"

#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math-glsl.h>

#include <scene/uniforms.glsl>

#include "instance-matrix.vs.glsl"

layout(location=VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=VERTEX_ATTRIBUTE_LOCATION_NORMAL) in vec3 vertex_normal;
layout(location=VERTEX_ATTRIBUTE_LOCATION_TANGENT) in vec3 vertex_tangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_BITANGENT) in vec3 vertex_bitangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_UV) in vec2 vertex_uv;


void transform_vertex()
{
  const mat4 model_matrix = get_model_matrix();
  
  vec4 world_coordinate = model_matrix * vec4(vertex_position, 1);
  
  vec4 world_normal = model_matrix * vec4(vertex_normal, 0);
  vec4 world_tangent = model_matrix * vec4(vertex_tangent, 0);
  vec4 world_bitangent = model_matrix * vec4(vertex_bitangent, 0);
    
  fragment.position = world_coordinate.xyz;
  fragment.normal = world_normal.xyz;
  fragment.tangent = world_tangent.xyz;
  fragment.bitangent = world_bitangent.xyz;
  fragment.uv = vertex_uv;
  
  gl_Position = scene.view_projection * world_coordinate;
}
