#ifndef CASCADEDGRID_UNIFORMS_GLSL
#define CASCADEDGRID_UNIFORMS_GLSL


#ifdef COMPUTE_GRIDS

struct CascadedGrids
{
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];

  // This are the samplers for rendering
  uint64_t padding[3];

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture[3];
};

#else // Vertex-/Fragment-Shader

struct CascadedGrids
{
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];
  usampler3D targetTexture[3];
  
  uint64_t padding[3];
};

#endif

#endif
