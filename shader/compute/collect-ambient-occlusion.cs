#version 450 core
#include <extensions/common.glsl>

#include <scene/uniforms.glsl>

#define N_GI_CONES 18
#include <voxels/cones-bouquet.glsl>

layout(local_size_x=GROUPS_SIZE_X, local_size_y=GROUPS_SIZE_Y, local_size_z=GROUPS_SIZE_Z) in;

#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
layout(rgba16ui)
#else
layout(r16ui)
#endif 
    writeonly uimage3D targetTexture;

struct found_leaf_t
{
#ifdef BVH_GRID_HAS_FOUR_COMPONENTS
  uvec4 index;
  vec4 occlusion;
#define found_leaf_real_t vec4
#else
  uint index;
  float occlusion;
#endif
};

    
void collect_scene_information_at(in vec3 world_pos, uvec3 voxel, float ao_length);

void main()
{
  uint whichTexture = gl_GlobalInvocationID.z / 16;
  uvec3 grid_cell = uvec3(gl_GlobalInvocationID.xy, gl_GlobalInvocationID.z%16);
  targetTexture = cascaded_grid_image(whichTexture);

  vec3 world_pos = cascaded_grid_cell_to_worldspace(grid_cell, whichTexture);
  
  collect_scene_information_at(world_pos, grid_cell, AO_RADIUS);
}

// =============================================================================

void init_found_leaf(out found_leaf_t leaf);
void take_better_node(inout found_leaf_t found_leaf, inout found_leaf_t next_candidate);

void collect_scene_information_at(in vec3 world_pos, uvec3 voxel, float ao_radius)
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
    
  }while(stack_depth>0);

  found_leaf_t found_leaf;
  init_found_leaf(found_leaf);
/*
  for(uint32_t i=0; i<num_leaves; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    uint32_t leaf = leaves[i];
    Sphere sphere = leaf_bounding_spheres[leaf];
    VoxelDataBlock* sdf = distance_field_data_blocks + leaf;
    
    take_better_node(found_leaf, found_leaf_distance, leaf, distance(sphere, world_pos));
    
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
  */
  
  imageStore(targetTexture, ivec3(voxel), uvec4(found_leaf.index));
}


#ifdef BVH_GRID_HAS_FOUR_COMPONENTS

void init_found_leaf(out found_leaf_t leaf)
{
  leaf.index = uvec4(0);
  leaf.occlusion = vec4(1.f);
}


void take_better_node(inout found_leaf_t found_leaf, inout found_leaf_t next_candidate)
{
}

#else

void init_found_leaf(out found_leaf_t leaf)
{
  leaf.index = 0;
  leaf.occlusion = 1.f;
}

void take_better_node(inout found_leaf_t found_leaf, inout found_leaf_t next_candidate)
{
}

#endif
