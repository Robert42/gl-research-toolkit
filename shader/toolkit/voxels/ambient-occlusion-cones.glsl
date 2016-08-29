#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <gl-noise/src/noise2D.glsl>

#define N_GI_CONES 9
#include <voxels/cones-bouquet.glsl>

int ao_distancefield_cost = 0;

bool intersects_with_cone_bouquet(in Sphere sphere, float cone_length)
{
  vec3 cone_origin = cone_bouquet[0].origin;
  
  float distance_to_sphere = max(0, distance(cone_origin, sphere.origin) - sphere.radius);
  
  float side_of_sphere = dot(sphere.origin+cone_normal*sphere.radius - cone_origin, cone_normal);
  
  return distance_to_sphere <= cone_length &&  side_of_sphere >= 0;
}

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
  
#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
  // In Range [intersection_distance_front, intersection_distance_back]
  float t = intersection_distance_front;
  
  int max_num_loops = 256;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
#else
  // In Range [0, 1]
  float exponential = 1.f/16.f;
  
  for(int i=0; i<4; ++i)
  {
    float t = mix(intersection_distance_front, intersection_distance_back, exponential);
#endif
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture) + distance(clamped_p, p);
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, t*inv_cone_length_voxelspace);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
    t += max(0.1f, abs(d));
#else
    exponential *= 2.f;
#endif
  }
  
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
  float V = 0.f;
  
  for(int i=0; i<N_GI_CONES; ++i)
    cone_bouquet_ao[i] = 1.f;
    
  #if defined(NO_BVH)
  ao_coneSoftShadow_bruteforce(bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #elif defined(BVH_WITH_STACK)
  ao_coneSoftShadow_bvh(bvh_bounding_spheres, bvh_nodes, bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #else
  #error UNKNOWN BVH usage
  #endif
    
  for(int i=0; i<N_GI_CONES; ++i)
    V += max(0, cone_bouquet_ao[i]);
    
  return V / N_GI_CONES;
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
