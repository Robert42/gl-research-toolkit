#ifndef CASCADEDGRID_UNIFORMS_GLSL
#define CASCADEDGRID_UNIFORMS_GLSL


#ifndef NUM_GRID_CASCADES
#error missing macro NUM_GRID_CASCADES
#endif
#if NUM_GRID_CASCADES>3
#error Unexpected number of cascades
#endif

struct CascadedGrids
{
  usampler3D gridTexture0;
#if NUM_GRID_CASCADES>1
  usampler3D gridTexture1;
#endif
#if NUM_GRID_CASCADES>2
  usampler3D gridTexture2;
#endif

#if BVH_USE_GRID_OCCLUSION
  sampler3D targetOcclusionTexture0;
#if NUM_GRID_CASCADES>1
  sampler3D targetOcclusionTexture1;
#endif
#if NUM_GRID_CASCADES>2
  sampler3D targetOcclusionTexture2;
#endif
#endif
  
// xyz are the grid origins and w the scale Factor for each grid.
  vec4 snappedGridLocation[NUM_GRID_CASCADES];
  vec4 smoothGridLocation[NUM_GRID_CASCADES];
};

#endif
