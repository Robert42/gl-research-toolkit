#include "output-block.vs.glsl"

#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math.h>

#include <scene/uniforms.glsl>

#ifndef AREA_LIGHT

struct Instance
{
  mat4 model_matrix;
};

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  Instance instance;
};

mat4 get_model_matrix()
{
  return instance.model_matrix;
}
#else
mat4 get_model_matrix()
{
  return mat4(1);
}
#endif

layout(location=VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=VERTEX_ATTRIBUTE_LOCATION_NORMAL) in vec3 vertex_normal;
layout(location=VERTEX_ATTRIBUTE_LOCATION_TANGENT) in vec3 vertex_tangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_BITANGENT) in vec3 vertex_bitangent;
layout(location=VERTEX_ATTRIBUTE_LOCATION_UV) in vec2 vertex_uv;


#ifdef PLAIN_COLOR

#include <pbs/pbs.glsl>

#ifndef DEPTH_PREPASS
#ifndef AREA_LIGHT
layout(binding=UNIFORM_BINDING_MATERIAL_INSTANCE_BLOCK, std140) uniform MaterialInstanceBlock
{
  vec3 base_color;
  float smoothness;
  vec3 emission;
  float metal_mask;
}material_instance;
#else
// #TODO
vec3 current_lightsource_luminance()
{
  return vec3(1,0,1);
}
#endif

out flat BaseMaterial plainColorMaterial;

#endif
#endif


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
  
#ifndef DEPTH_PREPASS
  
#ifdef PLAIN_COLOR
  BaseMaterial material;
  
#ifdef AREA_LIGHT
  material.smoothness = 0;
  material.base_color = vec3(0);
  material.metal_mask  = 0;
  material.emission = current_lightsource_luminance();
#else
  material.smoothness = material_instance.smoothness;
  material.base_color = material_instance.base_color;
  material.metal_mask  = material_instance.metal_mask;
  material.emission = material_instance.emission;
#endif
  material.normal = normalize(fragment.normal);
  material.reflectance = 0.5f;
  material.occlusion = 1;
  
  // No normal mapping here, so uv and tangent are unused
  
  plainColorMaterial = material;
#endif
#endif
}
