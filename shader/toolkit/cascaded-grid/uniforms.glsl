#ifndef CASCADEDGRID_UNIFORMS_GLSL
#define CASCADEDGRID_UNIFORMS_GLSL


#ifndef NUM_GRID_CASCADES
#error missing macro NUM_GRID_CASCADES
#endif
#if NUM_GRID_CASCADES>3
#error Unexpected number of cascades
#endif

#ifdef COMPUTE_GRIDS

struct CascadedGrids
{

  // This are the samplers for rendering  
  usampler3D padding0;
  #if NUM_GRID_CASCADES>1
  usampler3D padding1;
  #endif
  #if NUM_GRID_CASCADES>2
  usampler3D padding2;
  #endif

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture0;

#if NUM_GRID_CASCADES>1
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture1;
#endif
      
#if NUM_GRID_CASCADES>2
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif
      writeonly uimage3D targetTexture2;
#endif
      
  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[NUM_GRID_CASCADES];
};



#else // Vertex-/Fragment-Shader

struct CascadedGrids
{
  usampler3D gridTexture0;
#if NUM_GRID_CASCADES>1
  usampler3D gridTexture1;
#endif
#if NUM_GRID_CASCADES>2
  usampler3D gridTexture2;
#endif
  usampler3D padding0;
#if NUM_GRID_CASCADES>1
  usampler3D padding1;
#endif
#if NUM_GRID_CASCADES>2
  usampler3D padding2;
#endif
  
// xyz are the grid origins and w the scale Factor for each grid.
  vec4 gridLocation[NUM_GRID_CASCADES];
};

#endif

#endif
