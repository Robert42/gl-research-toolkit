#version 450 core
#include <extensions/common.glsl>

#include <glrt/glsl/math-glsl.h>
#include <glrt/glsl/layout-constants.h>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

#if defined(BVH_GRID_NEAREST_LEAF) || defined(BVH_GRID_NEAREST_LEAF_WITH_HEURISTIC_FOR_REST)
#define IMAGE_FORMAT_R16UI
#elif defined(BVH_GRID_NEAREST_FOUR_LEAVES) || defined(BVH_GRID_NEAREST_FOUR_LEAVES_WITH_HEURISTIC_FOR_REST)
#define IMAGE_FORMAT_RGBA16UI
#elif defined(NO_BVH) || defined(BVH_WITH_STACK)
// don't use this
#else
#error unexpected bvh method
#endif

struct CollectOcclusionMetaData
{
  // The first 3 images are used, the 4th image not
#ifdef IMAGE_FORMAT_R16UI
  layout(r16ui)
#else
  layout(rgba16ui)
#endif
      writeonly uimage3D targetTexture[4];
  // The first 3 vectors are the grid origins. The 4th vector is a vector of the grid scaleFactors for each grid.
  vec3 gridLocation[4];
};

layout(binding=UNIFORM_COLLECT_OCCLUSION_METADATA_BLOCK, std140)
uniform CollectOcclusionMetaDataBlock
{
  CollectOcclusionMetaData metaData;
};

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  
#ifdef IMAGE_FORMAT_R16UI
  layout(r16ui)
#else
  layout(rgba16ui)
#endif 
      writeonly uimage3D targetTexture = metaData.targetTexture[whichTexture];
  vec3 gridOrigin = metaData.gridLocation[whichTexture];
  float gridScaleFactor = metaData.gridLocation[3][whichTexture];
}
