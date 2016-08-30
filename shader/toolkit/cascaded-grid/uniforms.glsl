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

#if BVH_USE_GRID_OCCLUSION
  usampler3D padding_occlusion_0;
#if NUM_GRID_CASCADES>1
  usampler3D padding_occlusion_1;
#endif
#if NUM_GRID_CASCADES>2
  usampler3D padding_occlusion_2;
#endif
  layout(r8) writeonly image3D targetOcclusionTexture0;
#if NUM_GRID_CASCADES>1
  layout(r8) writeonly image3D targetOcclusionTexture1;
#endif
#if NUM_GRID_CASCADES>2
  layout(r8) writeonly image3D targetOcclusionTexture2;
#endif
#endif

  // xyz are the grid origins and w the scale Factor for each grid.
  vec4 snappedGridLocation[NUM_GRID_CASCADES];
  vec4 smoothGridLocation[NUM_GRID_CASCADES];
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

#if BVH_USE_GRID_OCCLUSION
  sampler3D targetOcclusionTexture0;
#if NUM_GRID_CASCADES>1
  sampler3D targetOcclusionTexture1;
#endif
#if NUM_GRID_CASCADES>2
  sampler3D targetOcclusionTexture2;
#endif
  sampler3D padding_occlusion_0;
#if NUM_GRID_CASCADES>1
  sampler3D padding_occlusion_1;
#endif
#if NUM_GRID_CASCADES>2
  sampler3D padding_occlusion_2;
#endif
#endif
  
// xyz are the grid origins and w the scale Factor for each grid.
  vec4 snappedGridLocation[NUM_GRID_CASCADES];
  vec4 smoothGridLocation[NUM_GRID_CASCADES];
};

#endif

#endif
