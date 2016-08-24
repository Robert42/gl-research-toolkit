#include <extensions/common.glsl>
#include <glrt/glsl/layout-constants.h>
#include <glrt/glsl/math-glsl.h>
#include <scene/uniforms.glsl>

layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1) in float vertex_parameter1;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR) in vec3 vertex_color;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2) in float vertex_parameter2;

out FragmentBlock
{
  vec3 color;
}fragment;

void pass_attributes_to_fragment_shader_no_transform(vec4 position, vec3 color)
{
  gl_Position = position;
  fragment.color = color;
}

void pass_attributes_to_fragment_shader_no_transform(vec3 position, vec3 color)
{
  pass_attributes_to_fragment_shader_no_transform(vec4(position, 1), color);
}

void pass_attributes_to_fragment_shader(vec3 position, vec3 color)
{
  vec4 transformed_position = scene.view_projection * vec4(position, 1);

  pass_attributes_to_fragment_shader_no_transform(transformed_position, color);
}

void pass_vertex_attributes_to_fragment_shader()
{
  pass_attributes_to_fragment_shader(vertex_position, vertex_color);
}
