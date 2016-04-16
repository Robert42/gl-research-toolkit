#include "output-block.vs.glsl"

#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math.h>

#include <scene/uniforms.vs.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  mat4 model_matrix;
};

layout(location=VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=VERTEX_ATTRIBUTE_LOCATION_NORMAL) in vec3 vertex_normal;
layout(location=VERTEX_ATTRIBUTE_LOCATION_TANGENT) in vec3 vertex_tangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_BITANGENT) in vec3 vertex_bitangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_UV) in vec2 vertex_uv;


#ifdef PLAIN_COLOR

#include <pbs/pbs.glsl>

layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_VERTEX_BLOCK, std140) uniform MaterialInstanceBlock
{
  vec3 base_color;
  float smoothness;
  vec3 emission;
  float metal_mask;
}material_instance;

out flat BaseMaterial plainColorMaterial;

#endif


void transform_vertex()
{
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
  
  
#ifdef PLAIN_COLOR
  BaseMaterial material;
  
  material.smoothness = material_instance.smoothness;
  material.base_color = material_instance.base_color;
  material.metal_mask  = material_instance.metal_mask;
  material.emission = material_instance.emission;
  material.normal = normalize(fragment.normal);
  material.reflectance = 0.5f;
  material.occlusion = 1;
  
  // No normal mapping here, so uv and tangent are unused
  
  plainColorMaterial = material;
#endif
}
