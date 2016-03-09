#include "output-block.vs.glsl"

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  mat4 model_matrix;
};

layout(binding=UNIFORM_BINDING_SCENE_VERTEX_BLOCK, std140) uniform SceneVertexBlock
{
  mat4 view_projection;
}scene;

layout(location=VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=VERTEX_ATTRIBUTE_LOCATION_NORMAL) in vec3 vertex_normal;
layout(location=VERTEX_ATTRIBUTE_LOCATION_TANGENT) in vec3 vertex_tangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_UV) in vec2 vertex_uv;


void transform_vertex()
{
  vec4 world_coordinate = model_matrix * vec4(vertex_position, 1);
  
  vec4 world_normal = model_matrix * vec4(vertex_normal, 0);
  vec4 world_tangent = model_matrix * vec4(vertex_tangent, 0);
    
  fragment.position = world_coordinate.xyz;
  fragment.normal = world_normal.xyz;
  fragment.tagent = world_tangent.xyz;
  fragment.uv = vertex_uv;
  
  gl_Position = scene.view_projection * world_coordinate;
}
