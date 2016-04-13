struct SceneVertexData
{
  mat4 view_projection;
};

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_SCENE_VERTEX_BLOCK, std140) uniform SceneVertexBlock
{
  SceneVertexData scene;
};
