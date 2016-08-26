#ifndef CASCADEDGRID_UNIFORMS_GLSL
#define CASCADEDGRID_UNIFORMS_GLSL


#ifdef COMPUTE_GRIDS

#if defined(BVH_GRID_NEAREST_LEAF) || defined(BVH_GRID_NEAREST_LEAF_WITH_HEURISTIC_FOR_REST)
#define IMAGE_FORMAT_R16UI
#elif defined(BVH_GRID_NEAREST_FOUR_LEAVES) || defined(BVH_GRID_NEAREST_FOUR_LEAVES_WITH_HEURISTIC_FOR_REST)
#define IMAGE_FORMAT_RGBA16UI
#elif defined(NO_BVH) || defined(BVH_WITH_STACK)
// don't use this
#else
#error unexpected bvh method
#endif

struct CascadedGrids
{
#ifdef IMAGE_FORMAT_R16UI
  layout(r16ui)
#else
  layout(rgba16ui)
#endif
      writeonly uimage3D targetTexture[3];
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];
};

#else // Vertex-/Fragment-Shader

struct CascadedGrids
{
  usampler3D targetTexture[3];
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];
};

#endif

#endif
