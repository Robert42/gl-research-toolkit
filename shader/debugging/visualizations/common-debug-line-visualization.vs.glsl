#include <glrt/glsl/layout-constants.h>

layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_POSITION) in vec3 vertex_position;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER1) in float vertex_parameter1;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_COLOR) in vec3 vertex_color;
layout(location=DEBUG_MESH_VERTEX_ATTRIBUTE_LOCATION_PARAMETER2) in float vertex_parameter2;

out FagmentBlock
{
  vec3 color;
}fragment;

layout(binding=UNIFORM_BINDING_SCENE_BLOCK, std140) uniform SceneBlock
{
  mat4 view_projection;
}scene;

void pass_attributes_to_fragment_shader(vec3 position, vec3 color)
{
  gl_Position = scene.view_projection * vec4(position, 1);
  fragment.color = color;
}

void pass_vertex_attributes_to_fragment_shader()
{
  pass_attributes_to_fragment_shader(vertex_position, vertex_color);
}
