
struct AoCollectHeader
{
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui) writeonly uimage3D targetTexture0;
  #if NUM_GRID_CASCADES>1
    layout(rgba16ui) writeonly uimage3D targetTexture1;
  #endif
  #if NUM_GRID_CASCADES>2
    layout(rgba16ui) writeonly uimage3D targetTexture2;
  #endif
#else
  layout(r16ui) writeonly uimage3D targetTexture0;
  #if NUM_GRID_CASCADES>1
    layout(r16ui) writeonly uimage3D targetTexture1;
  #endif
  #if NUM_GRID_CASCADES>2
    layout(r16ui) writeonly uimage3D targetTexture2;
  #endif
#endif

#if BVH_USE_GRID_OCCLUSION
  layout(r8) writeonly image3D targetOcclusionTexture0;
  #if NUM_GRID_CASCADES>1
    layout(r8) writeonly image3D targetOcclusionTexture1;
  #endif
  #if NUM_GRID_CASCADES>2
    layout(r8) writeonly image3D targetOcclusionTexture2;
  #endif
#endif

};

layout(binding=UNIFORM_COLLECT_OCCLUSION_METADATA_BLOCK, std140)
uniform AoCollectHeaderBLock
{
  AoCollectHeader header;
};


#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif 
    writeonly uimage3D cascaded_grid_image(uint i)
{
  /*
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  layout(rgba16ui)
#else
  layout(r16ui)
#endif 
    writeonly uimage3D targetTextures[NUM_GRID_CASCADES];
    
    targetTextures[0] = header.targetTexture0;
    #if NUM_GRID_CASCADES > 1
    targetTextures[1] = header.targetTexture1;
    #endif
    #if NUM_GRID_CASCADES > 2
    targetTextures[2] = header.targetTexture2;
    #endif
    
    return targetTextures[i];
    */
    
    if(i==0)
      return header.targetTexture0;
    #if NUM_GRID_CASCADES > 1
    else if(i==1)
      return header.targetTexture1;
    #endif
    #if NUM_GRID_CASCADES > 2
    else if(i==2)
      return header.targetTexture2;
    #endif
    return header.targetTexture0;
}

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
layout(rgba16ui)
#else
layout(r16ui)
#endif 
writeonly uimage3D leafIndexTexture;

#if BVH_USE_GRID_OCCLUSION
layout(r8) writeonly image3D occlusionTexture;
    
layout(r8) writeonly image3D cascaded_grid_image_occlusion(uint i)
{
  /*
  layout(r8) writeonly image3D targetOcclusionTextures[NUM_GRID_CASCADES];
    
  targetOcclusionTextures[0] = header.targetOcclusionTexture0;
  #if NUM_GRID_CASCADES > 1
  targetOcclusionTextures[1] = header.targetOcclusionTexture1;
  #endif
  #if NUM_GRID_CASCADES > 2
  targetOcclusionTextures[2] = header.targetOcclusionTexture2;
  #endif
  
  return targetOcclusionTextures[i];
  */
  
  if(i==0)
    return header.targetOcclusionTexture0;
  #if NUM_GRID_CASCADES > 1
  else if(i==1)
    return header.targetOcclusionTexture1;
  #endif
  #if NUM_GRID_CASCADES > 2
  else if(i==2)
    return header.targetOcclusionTexture2;
  #endif
  return header.targetOcclusionTexture0;
}
#endif


void get_cascaded_grid_images(uint whichTexture)
{
  leafIndexTexture = cascaded_grid_image(whichTexture);
  #if BVH_USE_GRID_OCCLUSION
  occlusionTexture = cascaded_grid_image_occlusion(whichTexture);
  #endif
}
