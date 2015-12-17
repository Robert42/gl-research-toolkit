#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140) uniform MeshInstanceBlock
{
  mat4 model_matrix;
}mesh_instance;

layout(binding=UNIFORM_BINDING_SCENE_BLOCK, std140) uniform SceneBlock
{
  mat4 view_projection;
  vec3 camera_position;
}scene;
