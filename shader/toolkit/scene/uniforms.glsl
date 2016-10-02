#ifndef SCENE_UNIFORMS_GLSL
#define SCENE_UNIFORMS_GLSL

#include <lighting/light-structs.glsl>
#include <voxels/voxel-structs.glsl>
#include <alignment.glsl>
#include <glrt/glsl/math-glsl.h>


#ifdef LOG_HEATVISION_DEBUG_COSTS
#define LOG_HEATVISION
#endif
#include <debugging/heat-vision.glsl>

#include <cascaded-grid/uniforms.glsl>

struct SceneLightData
{
  uint64_t sphere_arealights_address;
  uint64_t rect_arealights_address;
  uint32_t num_sphere_area_lights;
  uint32_t num_rect_area_lights;
};

struct SceneVoxelHeader
{
  uint64_t distance_field_bvh_inner_boundingsphere_array_address;
  uint64_t distance_field_bvh_node_array_address;
  uint64_t distance_field_boundingsphere_array_address;
  uint64_t distance_field_distancefielddata_array_address;
  // padding1(uint64_t, _padding1);
  uint32_t num_distance_fields;
  padding3(uint32_t, _padding2);
};

struct CandidateGridHeader
{
  usampler3D gridRanges;
  usampler2D _reservedForTiles;
  sampler3D fallbackSDF;
  sampler3D _padding;
  
  vec4 gridLocation;
  vec4 fallbackSdfGridLocation;
  
  uint64_t candidateGrid;
  uint64_t _candidate_reservedForTiles;
};

struct SceneData
{
  mat4 view_projection;
  vec3 camera_position;
  float totalTime;
  SceneLightData lights;
  SceneVoxelHeader voxelHeader;
  CandidateGridHeader candidateGridHeader;
  CascadedGrids cascadedGrids;
  
  uint32_t costsHeatvisionBlackLevel;
  uint32_t costsHeatvisionWhiteLevel;
  uint16_t bvh_debug_depth_begin;
  uint16_t bvh_debug_depth_end;
};

#include <glrt/glsl/layout-constants.h>

layout(binding=UNIFORM_BINDING_SCENE_BLOCK, std140) uniform SceneBlock
{
  SceneData scene;
};

// LIMIT_255
void get_sdfCandidates(in vec3 world_pos, out uint32_t num_static_candidates, out uint8_t* first_static_candidate, out uint32_t num_dynamic_candidates, out uint8_t* first_dynamic_candidate)
{
  vec3 gridLocationOffset = scene.candidateGridHeader.gridLocation.xyz;
  float gridLocationScale = scene.candidateGridHeader.gridLocation.w;
  
  ivec3 textureCoord = ivec3(world_pos * gridLocationScale + gridLocationOffset);
  textureCoord = clamp(textureCoord, ivec3(0), ivec3(SDF_CANDIDATE_GRID_SIZE));
  
  uint32_t gridRanges = texelFetch(scene.candidateGridHeader.gridRanges, textureCoord, 0).r;
  
  num_static_candidates = gridRanges>>24;
  // LIMIT_255
  first_static_candidate = (uint8_t*)(scene.candidateGridHeader.candidateGrid + (0x00ffffff & gridRanges));
  
  num_dynamic_candidates = 0;
}

sampler3D fallback_distance_field_data(out mat4x3 worldToVoxelSpace, out ivec3 voxelCount, out vec3 voxelToUvwSpace, out float worldToVoxelSpaceFactor)
{
  vec3 gridLocationOffset = scene.candidateGridHeader.gridLocation.xyz;
  float gridLocationScale = scene.candidateGridHeader.gridLocation.w;
  
  sampler3D texture = scene.candidateGridHeader.fallbackSDF;
  worldToVoxelSpace = mat4x3(gridLocationScale, 0, 0, 0, gridLocationScale, 0, 0, 0, gridLocationScale, gridLocationOffset);
  voxelCount = textureSize(texture, 0);
  voxelToUvwSpace = distance_field_voxelToUvwSpace(voxelCount);
  worldToVoxelSpaceFactor = gridLocationScale;
  
  return texture;
}

float blink(float rate=0.2)
{
  return step(rate, mod(scene.totalTime, rate*2.));
}

void get_sphere_lights(out uint32_t num_sphere_lights, out SphereAreaLight* sphere_lights)
{
  num_sphere_lights = scene.lights.num_sphere_area_lights;
  sphere_lights = (SphereAreaLight*)scene.lights.sphere_arealights_address;
}

void get_rect_lights(out uint32_t num_rect_lights, out RectAreaLight* rect_lights)
{
  num_rect_lights = scene.lights.num_rect_area_lights;
  rect_lights = (RectAreaLight*)scene.lights.rect_arealights_address;
}

uint32_t distance_fields_num()
{
  return scene.voxelHeader.num_distance_fields;
}

Sphere* distance_fields_bounding_spheres()
{
  return (Sphere*)scene.voxelHeader.distance_field_boundingsphere_array_address;
}

VoxelDataBlock* distance_fields_voxelData()
{
  return (VoxelDataBlock*)scene.voxelHeader.distance_field_distancefielddata_array_address;
}

Sphere* get_bvh_inner_bounding_spheres()
{
  return (Sphere*)scene.voxelHeader.distance_field_bvh_inner_boundingsphere_array_address;
}

uint16_t* get_bvh_inner_nodes()
{
  return (uint16_t*)scene.voxelHeader.distance_field_bvh_node_array_address;
}


// which_grid must be in 0..NUM_GRID_CASCADES
vec3 cascaded_grid_origin_snapped(uint which_grid)
{
  return scene.cascadedGrids.snappedGridLocation[which_grid].xyz;
}
vec3 cascaded_grid_origin_smooth(uint which_grid)
{
  return scene.cascadedGrids.smoothGridLocation[which_grid].xyz;
}

// which_grid must be in 0..NUM_GRID_CASCADES
float cascaded_grid_scale_factor(uint which_grid)
{
  return scene.cascadedGrids.snappedGridLocation[which_grid].w;
}

vec3 cascaded_grid_cell_to_worldspace(vec3 gridCoord, uint which_grid)
{
  return gridCoord / cascaded_grid_scale_factor(which_grid) + cascaded_grid_origin_snapped(which_grid);
}

vec3 cascaded_grid_cell_from_worldspace(vec3 world_pos, uint which_grid)
{
  return (world_pos - cascaded_grid_origin_snapped(which_grid)) * cascaded_grid_scale_factor(which_grid);
}

vec3 cascaded_grid_uvw_from_worldspace(vec3 world_pos, uint which_grid)
{
  return cascaded_grid_cell_from_worldspace(world_pos, which_grid) / 16.;
}

// Use this ONLY for the weights!!
vec3 cascaded_grid_cell_from_worldspace_smooth(vec3 world_pos, uint which_grid)
{
  return (world_pos - cascaded_grid_origin_smooth(which_grid)) * cascaded_grid_scale_factor(which_grid);
}

vec4 cascadedGridWeights(vec3 world_pos)
{
  vec4 weights = vec4(0);
  vec3 world_positions[NUM_GRID_CASCADES];
  
  // smoothing_distance
  const float d = 2.;
  
  // offset
  const float o = -0.5;
  // margin
  const float m = 2.0f;
  
  float left_weight = 1.f;
  
  for(int i=0; i<NUM_GRID_CASCADES; ++i)
  {
    vec3 grid_cell = cascaded_grid_cell_from_worldspace_smooth(world_pos, i);
    
    float w = min_component(smoothstep(vec3(o+m), vec3(o+d+m), grid_cell) * smoothstep(-vec3(o+16-m), -vec3(o+16-d-m), -grid_cell));
    w *= left_weight;
    
    left_weight -= w;
    
    weights[i] = w;
    
#define HIGHLIGHT_AREA_OUTSIDE_SNAPPED_BUT_WITH_WEIGHT_GT_0 0
#if HIGHLIGHT_AREA_OUTSIDE_SNAPPED_BUT_WITH_WEIGHT_GT_0 && (defined(CASCADED_GRID_WEIGHTS_TINTED) || defined(CASCADED_GRID_WEIGHTS))
    vec3 not_smooth = cascaded_grid_cell_from_worldspace(world_pos, i);
    
    if(w > 0 && clamp(not_smooth, o, 16+o)!=not_smooth)
      return vec3(1,0, 1);
#endif  
  }
  
  weights[3] = left_weight;
  
  return weights;
}

usampler3D cascaded_grid_texture(uint i)
{
  usampler3D targetTextures[MAX_NUM_GRID_CASCADES];
  
  targetTextures[0] = scene.cascadedGrids.gridTexture0;
  #if MAX_NUM_GRID_CASCADES > 1
  targetTextures[1] = scene.cascadedGrids.gridTexture1;
  #endif
  #if MAX_NUM_GRID_CASCADES > 2
  targetTextures[2] = scene.cascadedGrids.gridTexture2;
  #endif
  
  return targetTextures[i];
}

#if BVH_USE_GRID_OCCLUSION
sampler3D cascaded_grid_texture_occlusion(uint i)
{
  sampler3D targetOcclusionTextures[MAX_NUM_GRID_CASCADES];
    
  targetOcclusionTextures[0] = scene.cascadedGrids.targetOcclusionTexture0;
  #if MAX_NUM_GRID_CASCADES > 1
  targetOcclusionTextures[1] = scene.cascadedGrids.targetOcclusionTexture1;
  #endif
  #if MAX_NUM_GRID_CASCADES > 2
  targetOcclusionTextures[2] = scene.cascadedGrids.targetOcclusionTexture2;
  #endif
  
  return targetOcclusionTextures[i];
}
#endif

#if BVH_USE_GRID_OCCLUSION
float merged_cascaded_grid_texture_occlusion(vec3 world_pos)
{
  vec4 weights = cascadedGridWeights(world_pos);
  float grid_occlusion = textureLod(scene.cascadedGrids.targetOcclusionTexture0, cascaded_grid_uvw_from_worldspace(world_pos, 0), 0).r * weights.r
                            #if NUM_GRID_CASCADES > 1
                            + textureLod(scene.cascadedGrids.targetOcclusionTexture1, cascaded_grid_uvw_from_worldspace(world_pos, 1), 1).r * weights.g
                            #endif
                            #if NUM_GRID_CASCADES > 2
                            + textureLod(scene.cascadedGrids.targetOcclusionTexture2, cascaded_grid_uvw_from_worldspace(world_pos, 2), 2).r * weights.b
                            #endif
                            + weights.a;
  return grid_occlusion;
}
#endif

#ifndef highlightColor_DEFINED
vec4 heatvision(uint32_t value)
{
  uint32_t blackLevel = scene.costsHeatvisionBlackLevel;
  uint32_t whiteLevel = scene.costsHeatvisionWhiteLevel;
  blackLevel = min(whiteLevel, blackLevel);
  value = max(value, blackLevel);

  return heatvision(value-blackLevel, whiteLevel-blackLevel);
}
#define highlightColor_DEFINED
#endif

#endif // SCENE_UNIFORMS_GLSL
