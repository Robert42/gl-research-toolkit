#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  mat4 model_matrix;
}mesh_instance;

layout(binding=UNIFORM_BINDING_SCENE_BLOCK, std140) uniform SceneBlock
{
  mat4 view_projection;
  vec3 view_position;
}scene;

vec3 direction_to_camera(vec3 surface_position)
{
  return normalize(scene.view_position-surface_position);
}
