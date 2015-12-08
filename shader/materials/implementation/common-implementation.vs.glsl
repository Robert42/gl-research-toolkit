#include "output-block.vs.glsl"

#include "common-uniform-blocks.glsl"

layout(location=VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=VERTEX_ATTRIBUTE_LOCATION_NORMAL) in vec3 vertex_normal;
layout(location=VERTEX_ATTRIBUTE_LOCATION_TANGENT) in vec3 vertex_tangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_UV) in vec2 vertex_uv;

void transform_vertex()
{
  vec4 world_coordinate = mesh_instance.model_matrix * vec4(vertex_position, 1);
  vec4 world_normal = mesh_instance.model_matrix * vec4(vertex_normal, 0);
  vec4 world_tangent = mesh_instance.model_matrix * vec4(vertex_tangent, 0);
    
  fragment.position = world_coordinate.xyz;
  fragment.normal = world_normal.xyz;
  fragment.tagent = world_tangent.xyz;
  fragment.uv = vertex_uv;
  gl_Position = scene.view_projection * world_coordinate;
}
