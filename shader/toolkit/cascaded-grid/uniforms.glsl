#ifndef CASCADEDGRID_UNIFORMS_GLSL
#define CASCADEDGRID_UNIFORMS_GLSL


#ifdef COMPUTE_GRIDS

struct CascadedGrids
{

  // This are the samplers for rendering  
  usampler3D padding0;
  usampler3D padding1;
  usampler3D padding2;

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture0;
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture1;
      
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture2;
      
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];
  float _padding;
};

#else // Vertex-/Fragment-Shader

struct CascadedGrids
{
  // xyz are the grid origins and w the scale Factor for each grid.
  #if 0
  usampler3D targetTexture[4];
  #else
  usampler3D targetTexture0;
  usampler3D targetTexture1;
  usampler3D targetTexture2;
  usampler3D padding0;
  usampler3D padding1;
  usampler3D padding2;
  #endif
  
// xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[3];
  float _padding;
};

#endif

#endif
