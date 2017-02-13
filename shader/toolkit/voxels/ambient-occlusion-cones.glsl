#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <gl-noise/src/classicnoise2D.glsl>

#include <voxels/cones-bouquet.glsl>

int ao_distancefield_cost = 0;

/*
#define SDFSAMPLING_SPHERETRACING_START 0.f
#define SDFSAMPLING_SELF_SHADOW_AVOIDANCE 0.25f
#define SDFSAMPLING_EXPONENTIAL_NUM 4
#define SDFSAMPLING_EXPONENTIAL_START 0.25f
#define SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE (1.f/16.f)
#define SDFSAMPLING_EXPONENTIAL_FACTOR 2.f
#define SDFSAMPLING_EXPONENTIAL_OFFSET 0.f
#define AO_FALLBACK_NONE 0
#define AO_FALLBACK_CLAMPED 0
*/

#define NEED_AO_INTERPOLATION_STATIC defined(NO_BVH) && !AO_IGNORE_FALLBACK_SDF && !AO_FALLBACK_SDF_ONLY

void ao_falloff(inout float occlusionHeuristic, float sdf_value, float distance, float cone_length_voxelspace, float inv_cone_length_voxelspace, vec2 fallbackRangeVoxelspace, bool is_fallback)
{
#if AO_FALLBACK_NONE
  return;
#endif

#ifdef DISTANCEFIELD_AO_NUM_NEGATIVE_SAMPLES
    ao_distancefield_cost += int(sdf_value < 0);
#endif

  // linear falloff
  occlusionHeuristic = mix(occlusionHeuristic, 1.f, distance*inv_cone_length_voxelspace);

#if AO_FALLBACK_LINEAR
  return;
#endif

#if AO_FALLBACK_CLAMPED
//  occlusionHeuristic = mix(1, occlusionHeuristic, step(SDFSAMPLING_SELF_SHADOW_AVOIDANCE, distance));
  return;
#endif
  // smooth start of falloff
  occlusionHeuristic = mix(1, occlusionHeuristic, smoothstep(0.f, SDFSAMPLING_SELF_SHADOW_AVOIDANCE, distance));
  
#if NEED_AO_INTERPOLATION_STATIC
  if(is_fallback) // I hope the optimizer resolves this ;)
    occlusionHeuristic = mix(1.f, occlusionHeuristic, smoothstep(fallbackRangeVoxelspace[0], fallbackRangeVoxelspace[1], distance));
  else
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, smoothstep(fallbackRangeVoxelspace[0], fallbackRangeVoxelspace[1], distance));
#endif
}

#define AO_FALLOFF(occ, dist) ao_falloff(occ, d, dist, cone_length_voxelspace, inv_cone_length_voxelspace, fallbackRangeVoxelspace, is_fallback)

float ao_coneSoftShadow(in Cone cone, in sampler3D texture, in mat4x3 worldToVoxelSpace, in ivec3 voxelSize, in vec3 voxelToUvwSpace, in float worldToVoxelSpace_Factor, float intersection_distance_front, float intersection_distance_back, float cone_length, bool is_fallback=false)
{
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost++;
  #endif
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  vec2 fallbackRangeVoxelspace = worldToVoxelSpace_Factor * vec2(AO_STATIC_FALLBACK_FADING_START, AO_STATIC_FALLBACK_FADING_END);
  
  float cone_length_voxelspace = cone_length * worldToVoxelSpace_Factor;
  float inv_cone_length_voxelspace = 1.f / cone_length_voxelspace;
  
  intersection_distance_front = intersection_distance_front*worldToVoxelSpace_Factor;
  intersection_distance_back = min(intersection_distance_back*worldToVoxelSpace_Factor, cone_length_voxelspace);

#if !SPHERETRACING_BOUNDING_SPHERE_CLAMPING
  intersection_distance_front = 0;
  intersection_distance_back = cone_length_voxelspace;
#endif
    
  float minVisibility = 1.f;
  vec3 clamp_Range = vec3(voxelSize)-0.5f;
  
  // In Range [intersection_distance_front, intersection_distance_back]
  float exponential_start = max(SDFSAMPLING_EXPONENTIAL_START, intersection_distance_front);
  float spheretracing_start = max(SDFSAMPLING_SPHERETRACING_START, intersection_distance_front);

#if AO_FALLBACK_LINEAR || AO_FALLBACK_NONE
  exponential_start =  max(0, intersection_distance_front);
  spheretracing_start = max(0, intersection_distance_front);
#endif

#if AO_FALLBACK_CLAMPED
  spheretracing_start = max(spheretracing_start, SDFSAMPLING_SELF_SHADOW_AVOIDANCE);
#endif

  float t = spheretracing_start;
  
#if defined(DISTANCEFIELD_FIXED_SAMPLE_POINTS)
  // In Range [0, 1]
  float exponential = SDFSAMPLING_EXPONENTIAL_FIRST_SAMPLE;
  
  for(int i=0; i<SDFSAMPLING_EXPONENTIAL_NUM; ++i)
  {
  #if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
    t = mix(exponential_start, spheretracing_start, exponential);
  #else
    t = mix(exponential_start, intersection_distance_back, exponential);
  #endif
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    // ee882b37 the distance between the clamped position and the sampling position is now added to the distancefield distance
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture);
#if AO_SPHERETRACE_CLAMPING_CORRECTION
    d += distance(clamped_p, p);
#endif
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
    AO_FALLOFF(occlusionHeuristic, t);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
    exponential *= SDFSAMPLING_EXPONENTIAL_FACTOR;
    exponential += SDFSAMPLING_EXPONENTIAL_OFFSET;
  }
#endif

#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
#define GRADIENT_DESCENT 0
  int max_num_loops = SDFSAMPLING_SPHERE_TRACING_MAX_NUM_LOOPS;
#ifdef GRADIENT_DESCENT
  float dt = 0.f;
  float prev_d = -10000; // dt/dd will make the first value irrelevant (as long as dt is zero and dd is not zero)
  float dd; 
#endif
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    // ee882b37 the distance between the clamped position and the sampling position is now added to the distancefield distance
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture);
#if AO_SPHERETRACE_CLAMPING_CORRECTION
    d += distance(clamped_p, p);
#endif
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
    AO_FALLOFF(occlusionHeuristic, t);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
    float next_dt = abs(d);
    
#if GRADIENT_DESCENT
    dd = d - prev_d;
    float corrected_dt = d * dt/dd;
    prev_d = d;
#endif

#if GRADIENT_DESCENT
    dt = max(next_dt, mix(next_dt, corrected_dt, 1.e-5));
#else
    dt = next_dt;
#endif
    dt = max(AO_SPHERETRACE_MINSTEPSIZE, dt);
    t += dt;
  }
#endif
  
  return minVisibility;
}

float ao_coneSoftShadow(in Cone cone, in VoxelDataBlock* distance_field_data_block, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
#if NEED_AO_INTERPOLATION_STATIC
  intersection_distance_back = min(AO_STATIC_FALLBACK_FADING_END, intersection_distance_back);
#endif
  
  return ao_coneSoftShadow(cone, texture, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor, intersection_distance_front, intersection_distance_back, cone_length, false);
}


void ao_coneSoftShadow_fallbackGrid(float cone_length)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = fallback_distance_field_data(worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  
  float intersection_distance_front = 0.f;
  float intersection_distance_back = cone_length;
#if NEED_AO_INTERPOLATION_STATIC
  intersection_distance_front = AO_STATIC_FALLBACK_FADING_START;
#endif
  
  // TODO
  for(int j=0; j<N_GI_CONES; ++j)
  {
    Cone cone = cone_bouquet[j];
    float ao = ao_coneSoftShadow(cone, texture, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor, intersection_distance_front, intersection_distance_back, cone_length, true);
    cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao);
  }
}

void ao_coneSoftShadow_bruteforce(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length)
{
  for(uint32_t i=0; i<num_distance_fields; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    Sphere sphere = *bounding_spheres;
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      float distance_to_sphere_origin;
      
      #if defined(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST)
      ao_distancefield_cost++;
      #endif
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length) || AO_ENABLE_BOUNDINGSPHERE_CULLING==0)
      {
        #if defined(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_coneSoftShadow(cone_bouquet[j], distance_field_data_blocks, intersection_distance_front, intersection_distance_back, cone_length));
      }
    }
    
    ++bounding_spheres;
    ++distance_field_data_blocks;
  }
}

#if AO_CANDIDATE_GRID_CONTAINS_INDICES
void ao_coneSoftShadow_candidateGrid(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length)
{
  const vec3 world_pos = cone_bouquet[0].origin;
  uint32_t num_static_candidates;
  uint8_t* first_static_candidate;
  uint32_t num_dynamic_candidates;
  uint8_t* first_dynamic_candidate;
  get_sdfCandidates(world_pos, num_static_candidates, first_static_candidate, num_dynamic_candidates, first_dynamic_candidate);
  
  for(uint32_t i=0; i<num_static_candidates; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    uint16_t sdf_index = uint16_t(first_static_candidate[i]);
    Sphere sphere = bounding_spheres[sdf_index];
    VoxelDataBlock* sdf = distance_field_data_blocks + sdf_index;
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      float distance_to_sphere_origin;
      
      #if defined(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST)
      ao_distancefield_cost++;
      #endif
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length) || AO_ENABLE_BOUNDINGSPHERE_CULLING==0)
      {
        #if defined(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_coneSoftShadow(cone_bouquet[j], sdf, intersection_distance_front, intersection_distance_back, cone_length));
      }
    }
  }
}
#else
void ao_coneSoftShadow_candidateGrid(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length)
{
  const vec3 world_pos = cone_bouquet[0].origin;
  uint32_t num_static_candidates;
  CandidateType* first_static_candidate;
  uint32_t num_dynamic_candidates;
  uint8_t* first_dynamic_candidate;
  get_sdfCandidates(world_pos, num_static_candidates, first_static_candidate, num_dynamic_candidates, first_dynamic_candidate);
  
  for(uint32_t i=0; i<num_static_candidates; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    VoxelDataBlock* sdf = (VoxelDataBlock*)first_static_candidate;
    Sphere sphere = first_static_candidate->boundingSphere;
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      float distance_to_sphere_origin;
      
      #if defined(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST)
      ao_distancefield_cost++;
      #endif
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length) || AO_ENABLE_BOUNDINGSPHERE_CULLING==0)
      {
        #if defined(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_coneSoftShadow(cone_bouquet[j], sdf, intersection_distance_front, intersection_distance_back, cone_length));
      }
    }
    
    first_static_candidate++;
  }
}
#endif


#define BASE_VOXEL_GRID 1
#define PER_OBJECT_GRID 0

float ao_coneSoftShadow_cascaded_grids(in Sphere* leaf_bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length)
{
  const vec3 world_pos = cone_bouquet[0].origin;

#if NUM_GRID_CASCADES == 1
  const uint32_t N_voxel_weights = 9; // 1 + 1 * 8
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  const uint32_t N = 33; // 1 + 1 * 8 * 4
#else
  const uint32_t N = 9; // 1 + 1 * 8 * 1
#endif
#elif NUM_GRID_CASCADES == 2
  const uint32_t N_voxel_weights = 17; // 1 + 2 * 8
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  const uint32_t N = 65; // 1 + 2 * 8 * 4
#else
  const uint32_t N = 17; // 1 + 2 * 8 * 1
#endif
#elif NUM_GRID_CASCADES == 3
  const uint32_t N_voxel_weights = 25; // 1 + 3 * 8
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  const uint32_t N = 97; // 1 + 3 * 8 * 4
#else
  const uint32_t N = 25; // 1 + 3 * 8 * 1
#endif
#endif
  float voxel_weights[N_voxel_weights];
  uint16_t ids[N];
#if BASE_VOXEL_GRID
  ids[0] = uint16_t(0);
  voxel_weights[0] = 1.0;
  const uint32_t start = 1;
#else
  const uint32_t start = 0;
#endif
  uint32_t n = start;
  uint32_t n_voxel_weights = start;
  
  vec3 grid0_uvw = cascaded_grid_cell_from_worldspace(world_pos, 0);
  vec4 cascade_weights = cascadedGridWeights(world_pos);
  #if NUM_GRID_CASCADES>1
  vec3 grid1_uvw = cascaded_grid_cell_from_worldspace(world_pos, 1);
  #endif
  #if NUM_GRID_CASCADES>2
  vec3 grid2_uvw = cascaded_grid_cell_from_worldspace(world_pos, 2);
  #endif
  
  usampler3D t;
  ivec3 floor_uvw, ceil_uvw;
  
#define GATHER_INDICES(ID) \
  t = scene.cascadedGrids.gridTexture##ID; \
  floor_uvw = clamp(ivec3(floor(grid##ID##_uvw)), ivec3(0), ivec3(15)); \
  ceil_uvw = clamp(ivec3(ceil(grid##ID##_uvw)), ivec3(0), ivec3(15)); \
  for(uint32_t i=0; i<8; ++i) \
  { \
      vec3 voxel_pos = mix(floor_uvw, ceil_uvw, ivec3((i&4)>>2, (i&2)>>1, i&1)); \
      ivec3 uvw = ivec3(voxel_pos); \
      uvec4 index = texelFetch(t, uvw, 0); \
      float voxel_weight = 1.-sq_distance(voxel_pos, grid##ID##_uvw); \
      voxel_weights[n_voxel_weights++] = voxel_weight; \
      for(uint32_t i=0; i<BVH_GRID_NUM_COMPONENTS; ++i) \
        ids[n++] = uint16_t(mix(index[i], 0, step(num_distance_fields, index[i]))); \
  }
  
  GATHER_INDICES(0)
  #if NUM_GRID_CASCADES > 1
  GATHER_INDICES(1)
  #endif
  #if NUM_GRID_CASCADES > 2
  GATHER_INDICES(2)
  #endif
  
  for(uint32_t i=0; i<n; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    uint16_t leaf_index = ids[i];
    float voxel_weight = voxel_weights[i/BVH_GRID_NUM_COMPONENTS];
    
    float grid_interpolation = 1.f;//cascade_weights[((i-start)/(BVH_GRID_NUM_COMPONENTS*8)) % 3];
#if BASE_VOXEL_GRID || PER_OBJECT_GRID
    voxel_weight *= mix(1., grid_interpolation, step(PER_OBJECT_GRID+BASE_VOXEL_GRID, i));
#else
    voxel_weight *= grid_interpolation;
#endif

    
    Sphere sphere = leaf_bounding_spheres[leaf_index];
    VoxelDataBlock* sdf = distance_field_data_blocks + leaf_index;
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      Cone cone = cone_bouquet[j];
      float distance_to_sphere_origin;
      bool has_intersection = cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length) || AO_ENABLE_BOUNDINGSPHERE_CULLING==0;
      #if defined(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST)
      ao_distancefield_cost++;
      #endif
      if(has_intersection && voxel_weight>0)
      {
        #if defined(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        float ao_value = ao_coneSoftShadow(cone, sdf, intersection_distance_front, intersection_distance_back, cone_length);
        ao_value = mix(1.0, ao_value, voxel_weight);
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_value);
      }
    }
  }
  
#if BVH_USE_GRID_OCCLUSION
  float grid_occlusion = merged_cascaded_grid_texture_occlusion(world_pos);
  return grid_occlusion;
#else
  return 1.f;
#endif
}

void ao_coneSoftShadow_bvh(in Sphere* bvh_inner_bounding_sphere, uint16_t* inner_nodes, in Sphere* leaf_bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length)
{
  uint16_t stack[BVH_MAX_STACK_DEPTH];
  stack[0] = uint16_t(0);
  uint32_t stack_depth=1;

  uint16_t leaves[BVH_MAX_VISITED_LEAVES];
  uint32_t num_leaves;
  #if HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE!=0 && HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE!=1
  #error invalid value for HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE, expecter 0 or 1
  #endif
  for(uint16_t i=uint16_t(0); i<uint16_t(HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE); ++i)
    leaves[i] = i;
  num_leaves = uint16_t(HOLD_BACK_HUGE_LEAf_FROM_BVH_TREE);

  do {
    stack_depth--;
    uint32_t current_node = stack[stack_depth];
    
    uint16_t* child_nodes = inner_nodes + current_node*2;
    uint32_t left_node = child_nodes[0];
    uint32_t right_node = child_nodes[1];
    
    uint32_t left_is_leaf = (left_node & 0x8000) >> 15;
    uint32_t right_is_leaf = (right_node & 0x8000) >> 15;
    
    uint32_t left_is_inner_node = uint32_t(1) ^ left_is_leaf;
    uint32_t right_is_inner_node = uint32_t(1) ^ right_is_leaf;

    left_node = left_node & uint32_t(0x7fff);
    right_node = right_node & uint32_t(0x7fff);
    
    Sphere left_sphere;
    left_sphere.origin = mix(leaf_bounding_spheres[left_node].origin, bvh_inner_bounding_sphere[left_node].origin, left_is_inner_node);
    left_sphere.radius = mix(leaf_bounding_spheres[left_node].radius, bvh_inner_bounding_sphere[left_node].radius, left_is_inner_node);
    Sphere right_sphere;
    right_sphere.origin = mix(leaf_bounding_spheres[right_node].origin, bvh_inner_bounding_sphere[right_node].origin, right_is_inner_node);
    right_sphere.radius = mix(leaf_bounding_spheres[right_node].radius, bvh_inner_bounding_sphere[right_node].radius, right_is_inner_node);
    
    uint32_t conebouquet_intersects_left = uint32_t(intersects_with_cone_bouquet(left_sphere, cone_length));
    uint32_t conebouquet_intersects_right = uint32_t(intersects_with_cone_bouquet(right_sphere, cone_length));
    
    stack[stack_depth] = uint16_t(left_node);
    stack_depth += left_is_inner_node & conebouquet_intersects_left;
    #if BVH_MAX_STACK_DEPTH < MAX_NUM_STATIC_MESHES
    stack_depth = min(stack_depth, BVH_MAX_STACK_DEPTH-1);
    #endif
    stack[stack_depth] = uint16_t(right_node);
    stack_depth += right_is_inner_node & conebouquet_intersects_right;
    #if BVH_MAX_STACK_DEPTH < MAX_NUM_STATIC_MESHES
    stack_depth = min(stack_depth, BVH_MAX_STACK_DEPTH-1);
    #endif
    
    leaves[num_leaves] = uint16_t(right_node);
    num_leaves += right_is_leaf & conebouquet_intersects_right;
    #if BVH_MAX_VISITED_LEAVES < MAX_NUM_STATIC_MESHES
    num_leaves = min(num_leaves, BVH_MAX_VISITED_LEAVES-1);
    #endif
    leaves[num_leaves] = uint16_t(left_node);
    num_leaves += left_is_leaf & conebouquet_intersects_left;
    #if BVH_MAX_VISITED_LEAVES < MAX_NUM_STATIC_MESHES
    num_leaves = min(num_leaves, BVH_MAX_VISITED_LEAVES-1);
    #endif
    
  }while(stack_depth>0);


  for(uint32_t i=0; i<num_leaves; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    Sphere sphere = leaf_bounding_spheres[leaves[i]];
    VoxelDataBlock* sdf = distance_field_data_blocks + leaves[i];
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      Cone cone = cone_bouquet[j];
      float distance_to_sphere_origin;
      bool has_intersection = cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length) || AO_ENABLE_BOUNDINGSPHERE_CULLING==0;
      #if defined(DISTANCEFIELD_AO_COST_CONE_SPHERE_INTERSECTION_TEST)
      ao_distancefield_cost++;
      #endif
      if(has_intersection)
      {
        #if defined(DISTANCEFIELD_AO_COST_NUM_CONETRACED_SDF)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        
        float ao = ao_coneSoftShadow(cone_bouquet[j], sdf, intersection_distance_front, intersection_distance_back, cone_length);
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao);
      }
    }
  }
}

float distancefield_ao()
{
  const float ao_radius = AO_RADIUS;
  
  Sphere* bvh_bounding_spheres = get_bvh_inner_bounding_spheres();
  uint16_t* bvh_nodes = get_bvh_inner_nodes();
  Sphere* bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  float occlusion_factor = 1.f;
  init_cone_bouquet_ao();
  
  #if defined(NO_BVH)
  
    #if !AO_IGNORE_FALLBACK_SDF
      ao_coneSoftShadow_fallbackGrid(ao_radius);
    #endif
    
    #if !AO_FALLBACK_SDF_ONLY
    
      #if AO_USE_CANDIDATE_GRID
      ao_coneSoftShadow_candidateGrid(bounding_spheres, distance_field_data_blocks, num_distance_fields, ao_radius);
      #else
      ao_coneSoftShadow_bruteforce(bounding_spheres, distance_field_data_blocks, num_distance_fields, ao_radius);
      #endif
  
    #endif
  
  #elif defined(BVH_WITH_STACK)
    ao_coneSoftShadow_bvh(bvh_bounding_spheres, bvh_nodes, bounding_spheres, distance_field_data_blocks, num_distance_fields, ao_radius);
  #elif defined(BVH_USE_GRID)
    occlusion_factor = ao_coneSoftShadow_cascaded_grids(bounding_spheres, distance_field_data_blocks, num_distance_fields, ao_radius);
  #else
    #error UNKNOWN BVH usage
  #endif
    
  return accumulate_bouquet_to_total_occlusion() * occlusion_factor;
}
