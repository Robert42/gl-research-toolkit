#version 450 core
#include <extensions/common.glsl>

#include "common-debug-line-visualization.vs.glsl"

#include <voxels/voxel-structs.glsl>


struct VoxelBoundingBox
{
  mat4 worldToVoxelSpace;
  ivec3 voxelCount;
};

layout(binding=UNIFORM_BINDING_MESH_INSTANCE_BLOCK, std140)
uniform PositionBlock
{
  VoxelBoundingBox voxelData;
};


void main()
{
  vec3 grid_position = vertex_color + vertex_position * vec3(voxelData.voxelCount);
  
  vec3 ws_position = transform_point(inverse(voxelData.worldToVoxelSpace), grid_position);
  
  if(any(greaterThan(grid_position, voxelData.voxelCount)))
    ws_position = vec3(0);

  pass_attributes_to_fragment_shader(ws_position, vertex_color);
}
