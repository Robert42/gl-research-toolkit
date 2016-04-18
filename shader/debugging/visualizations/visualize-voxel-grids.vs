#version 450 core
#include <extensions/common.glsl>

#include "common-debug-line-visualization.vs.glsl"

#include <geometry/transform.glsl>

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform PositionBlock
{
  uint64_t gpuTextureHandle;
  mat4 worldToVoxelSpace;
  ivec3 voxelCount;
};


void main()
{
  vec3 ws_position = vertex_position;

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
