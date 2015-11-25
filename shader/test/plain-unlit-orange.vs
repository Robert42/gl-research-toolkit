#version 450 core

#include "../scene/transformation.glsl"


uniform MeshBlock
{
  mat4 model_matrix;
} mesh;


uniform SceneBlock
{
  mat4 view_projection;
} scene;


in vec2 position;

void main()
{
  gl_Position = scene.view_projection * mesh.model_matrix * vec4(position.x, position.y, 0, 1);
}
