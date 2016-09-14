#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <gl-noise/src/noise2D.glsl>

#include <voxels/cones-bouquet.glsl>

int ao_distancefield_cost = 0;

/*
#define SDFSAMPLING_SPHERETRACING_START 0.f
#define SDFSAMPLING_SELF_SHADOW_AVOIDANCE 0.25f
#define SDFSAMPLING_EXPONENTIAL_NUM 4
#define SDFSAMPLING_EXPONENTIAL_START (1.f/16.f)
#define SDFSAMPLING_EXPONENTIAL_FACTOR 2.f
#define SDFSAMPLING_EXPONENTIAL_OFFSET 0.f
*/

float ao_coneSoftShadow(in Cone cone, in VoxelDataBlock* distance_field_data_block, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost++;
  #endif
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  float cone_length_voxelspace = cone_length * worldToVoxelSpace_Factor;
  float inv_cone_length_voxelspace = 1.f / cone_length_voxelspace;
  
  float self_shadow_avoidance = 0.25f; // TODO: use the distancefield itself to get the best offset? // TODO: use the id to deicide, whether the self occlusion offset should be applied?
  
  intersection_distance_front = max(intersection_distance_front*worldToVoxelSpace_Factor, self_shadow_avoidance);
  intersection_distance_back = min(intersection_distance_back*worldToVoxelSpace_Factor, cone_length_voxelspace);
    
  float minVisibility = 1.f;
  vec3 clamp_Range = vec3(voxelSize)-0.5f;
  
  // In Range [intersection_distance_front, intersection_distance_back]
  float spheretracing_start = max(SDFSAMPLING_SPHERETRACING_START, intersection_distance_front);
  float t = spheretracing_start;
  
#if defined(DISTANCEFIELD_FIXED_SAMPLE_POINTS)
  // In Range [0, 1]
  float exponential = SDFSAMPLING_EXPONENTIAL_START;
  
  for(int i=0; i<SDFSAMPLING_EXPONENTIAL_NUM; ++i)
  {
  #if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
    t = mix(intersection_distance_front, spheretracing_start, exponential);
  #else
    t = mix(intersection_distance_front, intersection_distance_back, exponential);
  #endif
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    // ee882b37 the distance between the clamped position and the sampling position is now added to the distancefield distance
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture) + distance(clamped_p, p);
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, t*inv_cone_length_voxelspace);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
    exponential *= SDFSAMPLING_EXPONENTIAL_FACTOR;
    exponential += SDFSAMPLING_EXPONENTIAL_OFFSET;
  }
#endif

#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
  int max_num_loops = 256;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    // ee882b37 the distance between the clamped position and the sampling position is now added to the distancefield distance
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture) + distance(clamped_p, p);
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
    // linear falloff
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, t*inv_cone_length_voxelspace);
    // smooth start of falloff
    occlusionHeuristic = mix(1, occlusionHeuristic, smoothstep(0.f, SDFSAMPLING_SELF_SHADOW_AVOIDANCE, t));
    minVisibility = min(minVisibility, occlusionHeuristic);
    
    t += max(0.1f, abs(d));
  }
#endif


  
  return minVisibility;
}

void ao_coneSoftShadow_bruteforce(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
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
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length))
      {
        #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
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

#define BASE_VOXEL_GRID 1
#define PER_OBJECT_GRID 0

float ao_coneSoftShadow_cascaded_grids(in Sphere* leaf_bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
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
      bool has_intersection = cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length);
      if(has_intersection && voxel_weight>0)
      {
        #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
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

void ao_coneSoftShadow_bvh(in Sphere* bvh_inner_bounding_sphere, uint16_t* inner_nodes, in Sphere* leaf_bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  uint16_t stack[BVH_MAX_STACK_DEPTH];
  stack[0] = uint16_t(0);
  uint32_t stack_depth=1;

  uint16_t leaves[BVH_MAX_VISITED_LEAVES];
  uint32_t num_leaves;
  num_leaves = 0;

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
      bool has_intersection = cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length);
      if(has_intersection)
      {
        #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
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

float distancefield_ao(in Sphere* bvh_bounding_spheres, uint16_t* bvh_nodes, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float radius=3.5)
{
  float occlusion_factor = 1.f;
  init_cone_bouquet_ao();
    
  #if defined(NO_BVH)
  ao_coneSoftShadow_bruteforce(bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #elif defined(BVH_WITH_STACK)
  ao_coneSoftShadow_bvh(bvh_bounding_spheres, bvh_nodes, bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #elif defined(BVH_USE_GRID)
  occlusion_factor = ao_coneSoftShadow_cascaded_grids(bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #else
  #error UNKNOWN BVH usage
  #endif
    
  return accumulate_bouquet_to_total_occlusion() * occlusion_factor;
}

float distancefield_ao(float radius=AO_RADIUS)
{
  Sphere* _bvh_bounding_spheres = get_bvh_inner_bounding_spheres();
  uint16_t* _bvh_nodes = get_bvh_inner_nodes();
  Sphere* _bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* _distance_field_data_blocks = distance_fields_voxelData();
  uint32_t _num_distance_fields = distance_fields_num();
  
  return distancefield_ao(_bvh_bounding_spheres, _bvh_nodes, _bounding_spheres, _distance_field_data_blocks, _num_distance_fields, radius);
}
