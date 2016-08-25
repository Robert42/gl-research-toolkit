#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math-glsl.h>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

struct CollectOcclusionMetaData
{
  // The first 3 images are used, the 4th image not
  layout(r32f) image3D targetTexture[4];
  // The first 3 vectors are the grid origins. The 4th vector is a vector of the grid scaleFactors for each grid.
  vec3 gridLocation[4];
};

layout(binding=0, std140)
uniform CollectOcclusionMetaDataBlock
{
  CollectOcclusionMetaData metaData;
};

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  
  layout(r32f) image3D targetTexture = metaData.targetTexture[whichTexture];
  vec3 gridOrigin = metaData.gridLocation[whichTexture];
  float gridScaleFactor = metaData.gridLocation[3][whichTexture];
}
