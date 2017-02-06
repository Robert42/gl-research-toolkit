#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>
#include "ao-collect-uniform.glsl"

#define COMPUTE_SHADER

#define USE_CONES 1
#define USE_ONLY_LEAF_SPHERE 0

#if USE_CONES
#if N_GI_CONES==9
#undef N_GI_CONES
#define N_GI_CONES 18
#elif N_GI_CONES==7
#undef N_GI_CONES
#define N_GI_CONES 14
#else
#error unexpected number cones
#endif
#include <voxels/ambient-occlusion-cones.glsl>
#endif

#if 0==USE_CONES && BVH_USE_GRID_OCCLUSION!=0
#error BVH_USE_GRID_OCCLUSION without cones is not possible
#endif


layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;


struct found_leaf_t
{
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  uvec4 index;
  vec4 occlusion;
#else
  uint index;
  float occlusion;
#endif
};
    
void collect_scene_information_at(in vec3 world_pos, ivec3 voxel, float cell_size, float ao_length);

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  ivec3 grid_cell = ivec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z%16);
  get_cascaded_grid_images(whichTexture);

  float cell_size = 1. / cascaded_grid_scale_factor(whichTexture);

  vec3 world_pos = cascaded_grid_cell_to_worldspace(grid_cell, whichTexture);

#if USE_CONES
  init_cone_bouquet(mat3(1), world_pos);
#endif
  
#if 1
  collect_scene_information_at(world_pos, grid_cell, cell_size, AO_RADIUS);
#else
  imageStore(leafIndexTexture, grid_cell, uvec4(grid_cell*16, whichTexture));
  #if BVH_USE_GRID_OCCLUSION
  imageStore(occlusionTexture, grid_cell, vec4(vec3(grid_cell)/16., 1));
  #endif
#endif
}

// =============================================================================

void init_found_leaf(out found_leaf_t leaf);
void take_better_node(inout found_leaf_t found_leaf, in uint32_t index, in float occlusion);

void collect_scene_information_at(in vec3 world_pos, ivec3 voxel, float cell_size, float ao_radius)
{
  Sphere* bvh_inner_bounding_sphere = get_bvh_inner_bounding_spheres();
  uint16_t* inner_nodes = get_bvh_inner_nodes();
  Sphere* leaf_bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* leaf_data_blocks = distance_fields_voxelData();
  
  Sphere cellInfluence;
  cellInfluence.origin = world_pos;
  cellInfluence.radius = ao_radius;
  
  
  uint16_t stack[BVH_MAX_STACK_DEPTH];
  stack[0] = uint16_t(0);
  uint32_t stack_depth=1;

  uint16_t leaves[BVH_MAX_VISITED_LEAVES];
  uint16_t leaves_occlusion[BVH_MAX_VISITED_LEAVES];
  uint32_t num_leaves;
  num_leaves = 0;
  
  uint32_t is_within_a_leaf = 0;

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
    
    uint32_t conebouquet_intersects_left = uint32_t(intersects(left_sphere, cellInfluence));
    uint32_t conebouquet_intersects_right = uint32_t(intersects(right_sphere, cellInfluence));
    
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
    
    uint32_t left_contains_world_pos = 1 ^ uint32_t(step( left_sphere.radius+cell_size, distance(left_sphere.origin,world_pos) ));
    uint32_t right_contains_world_pos = 1 ^ uint32_t(step( right_sphere.radius+cell_size, distance(right_sphere.origin,world_pos) ));
    is_within_a_leaf += (left_is_leaf & left_contains_world_pos) | (right_is_leaf & right_contains_world_pos);
    
  }while(stack_depth>0);
  
  // the cell is not wothin a leaf, there's no geometry here which will  be shaded in the fragment shader phase => do not waste going through the leafs and accessing the texture memory
  if(is_within_a_leaf == 0)
    return;

  found_leaf_t found_leaf;
  init_found_leaf(found_leaf);

#if USE_CONES
  init_cone_bouquet_ao();
#endif
  
  const float cone_length = ao_radius;

  for(uint32_t i=0; i<num_leaves; ++i)
  {
    uint32_t leaf = leaves[i];
    Sphere sphere = leaf_bounding_spheres[leaf];
    VoxelDataBlock* sdf = leaf_data_blocks + leaf;
    
#if USE_CONES
    float V = 0.f;
    for(int j=0; j<N_GI_CONES; ++j)
    {
      Cone cone = cone_bouquet[j];
      float distance_to_sphere_origin;
      bool has_intersection = cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length);
      if(has_intersection)
      {
        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        
        float ao = ao_coneSoftShadow(cone, sdf, intersection_distance_front, intersection_distance_back, cone_length);
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao);
        
        // TODO make this clamping optional? (negative values might be preferred)
        V += max(0, ao);
      }
    }
    
    V /= N_GI_CONES;
#endif

#if USE_ONLY_LEAF_SPHERE
    float V = -sq_distance(sphere.origin, world_pos);
#endif
    
    take_better_node(found_leaf, leaf, V);
  }

#if USE_CONES
  #ifndef BVH_GRID_UNCLAMPED_OCCLUSION
  const float total_ao_at = accumulate_bouquet_to_total_occlusion_unclamped();
  #else
  const float total_ao_at = accumulate_bouquet_to_total_occlusion();
  #endif
#endif
  
  imageStore(leafIndexTexture, voxel, uvec4(found_leaf.index));
#if BVH_USE_GRID_OCCLUSION
  imageStore(occlusionTexture, voxel, vec4(total_ao_at));
#endif
}


#ifdef BVH_GRID_HAS_FOUR_COMPONENTS

void init_found_leaf(out found_leaf_t leaf)
{
  leaf.index = uvec4(0);
  leaf.occlusion = vec4(1.f);
}

void take_better_node(inout found_leaf_t found_leaf, in uint32_t index, in float occlusion)
{
  int i = index_of_max_component(found_leaf.occlusion);
  
  float new_is_better = step(found_leaf.occlusion[i], occlusion);
  
  found_leaf.occlusion[i] = min(found_leaf.occlusion[i], occlusion);
  found_leaf.index[i] = uint32_t(mix(found_leaf.index[i], index, uint32_t(new_is_better)));
}

#else

void init_found_leaf(out found_leaf_t leaf)
{
  leaf.index = 0;
  leaf.occlusion = 1.f;
}

void take_better_node(inout found_leaf_t found_leaf, in uint32_t index, in float occlusion)
{
  float new_is_better = step(found_leaf.occlusion, occlusion);
  
  found_leaf.occlusion = min(found_leaf.occlusion, occlusion);
  found_leaf.index = uint32_t(mix(found_leaf.index, index, uint32_t(new_is_better)));
}

#endif
