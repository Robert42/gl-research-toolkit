struct SceneVertexData
{
  mat4 view_projection;
};

layout(binding=UNIFORM_BINDING_SCENE_VERTEX_BLOCK, std140) uniform SceneVertexBlock
{
  SceneVertexData scene;
};
