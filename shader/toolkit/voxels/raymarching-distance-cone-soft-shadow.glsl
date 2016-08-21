#ifndef RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL
#define RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL

#include <scene/uniforms.glsl>

#include "distance-field-utils.glsl"

#include <cone-tracing/cone-occlusion.glsl>

float coneSoftShadow_singleVoxel(in Cone cone, in VoxelDataBlock* distance_field_data_block, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  float cone_length_voxelspace = cone_length * worldToVoxelSpace_Factor;
  float inv_cone_length_voxelspace = 1.f / cone_length_voxelspace;
  
  float self_shadow_avoidance = 0.25f; // TODO: use the distancefield itself to get the best offset? // TODO: use the id to deicide, whether the self occlusion offset should be applied?
  
  intersection_distance_front = max(intersection_distance_front*worldToVoxelSpace_Factor, self_shadow_avoidance);
  intersection_distance_back = min(intersection_distance_back*worldToVoxelSpace_Factor, cone_length_voxelspace);
    
  float t = intersection_distance_front;
  
  float minVisibility = 1.f;
  vec3 clamp_Range = vec3(voxelSize)-0.5f;
  
  int max_num_loops = 1024;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture) + distance(clamped_p, p);
    float cone_radius = cone.tan_half_angle * t;
    
    minVisibility = min(minVisibility, coneOcclusionHeuristic(cone_radius, d));
    
    t += abs(d);
  }
  
  return minVisibility;
}

float coneSoftShadow_array_of_leaves(in Cone cone, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  float occlusion = 1.f;
  
  for(uint32_t i=0; i<num_distance_fields; ++i)
  {
    Sphere sphere = *bounding_spheres;
    
    float distance_to_sphere_origin;
    if(cone_intersects_sphere(cone, sphere, distance_to_sphere_origin))
    {
      float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
      float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
      occlusion = min(occlusion, coneSoftShadow_singleVoxel(cone, distance_field_data_blocks, intersection_distance_front, intersection_distance_back, cone_length));
    }
    
    ++bounding_spheres;
    ++distance_field_data_blocks;
  }
  
  return occlusion;
}

float smooth_shadow_occlusion_value(float occlusion)
{
  occlusion = max(0, occlusion);

  occlusion = sq(occlusion);
  occlusion = smoothstep(0, 1, occlusion);

  return occlusion;
}

float coneSoftShadow_bvh_leaf(in Cone cone, in uint16_t leaf_node, in Sphere* leaf_bounding_spheres, in VoxelDataBlock* leaf_distance_field_data_blocks, float cone_length=inf)
{
  float distance_to_sphere_origin;
  Sphere sphere = leaf_bounding_spheres[leaf_node];
  if(cone_intersects_sphere(cone, sphere, distance_to_sphere_origin))
  {
    float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
    float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
    return coneSoftShadow_singleVoxel(cone, leaf_distance_field_data_blocks+leaf_node, intersection_distance_front, intersection_distance_back, cone_length);
  }else
    return 1.f;
}

float coneSoftShadow_bvh_iterative(in Cone cone, in uint16_t root_node, in uint16_t* inner_nodes, in Sphere* bvh_inner_bounding_sphere, in Sphere* leaf_bounding_spheres, in VoxelDataBlock* leaf_distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  // TODO try, whether using uint8_t instead of uint16_t has more performance
  
  uint16_t stack[BVH_MAX_STACK_DEPTH];
  stack[0] = root_node;
  uint16_t stack_depth=uint16_t(1);
  
  uint16_t leaves[BVH_MAX_VISITED_LEAVES];
  uint16_t num_leaves = uint16_t(0);
  
  do {
    stack_depth--;
    uint16_t current_node = stack[stack_depth];
    
    uint16_t* child_nodes = inner_nodes + current_node*uint16_t(2);
    uint16_t left_node = child_nodes[0];
    uint16_t right_node = child_nodes[1];
    
    bool left_is_inner_node = (left_node & uint16_t(0x8000)) == uint16_t(0);
    bool right_is_inner_node = (right_node & uint16_t(0x8000)) == uint16_t(0);

    left_node = left_node & uint16_t(0x7fff);
    right_node = right_node & uint16_t(0x7fff);
    
    if(left_is_inner_node)
    {
      float d;
      if(cone_intersects_sphere(cone, bvh_inner_bounding_sphere[left_node], d))
      {
        stack[stack_depth++] = left_node;
        #if BVH_MAX_STACK_DEPTH < MAX_NUM_STATIC_MESHES
        stack_depth = min(stack_depth, BVH_MAX_STACK_DEPTH-1);
        #endif
      }
    }else
    {
      leaves[num_leaves++] = left_node;
      #if BVH_MAX_VISITED_LEAVES < MAX_NUM_STATIC_MESHES
      num_leaves = min(num_leaves, BVH_MAX_VISITED_LEAVES-1);
      #endif
    }
      
    if(right_is_inner_node)
    {
      float d;
      if(cone_intersects_sphere(cone, bvh_inner_bounding_sphere[right_node], d))
      {
        stack[stack_depth++] = right_node;
        #if BVH_MAX_STACK_DEPTH < MAX_NUM_STATIC_MESHES
        stack_depth = min(stack_depth, BVH_MAX_STACK_DEPTH-1);
        #endif
      }
    }else
    {
      leaves[num_leaves++] = right_node;
      #if BVH_MAX_VISITED_LEAVES < MAX_NUM_STATIC_MESHES
      num_leaves = min(num_leaves, BVH_MAX_VISITED_LEAVES-1);
      #endif
    }
    
  }while(stack_depth>uint16_t(0));
  
  float occlusion = 1.f;
  for(uint16_t i=uint16_t(0); i<num_leaves; ++i)
  {
    occlusion = min(occlusion, coneSoftShadow_bvh_leaf(cone, leaves[i], leaf_bounding_spheres, leaf_distance_field_data_blocks, cone_length));
  }

  return occlusion;
}

#if defined(NO_BVH)

float coneSoftShadow(in Cone cone, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  return coneSoftShadow_array_of_leaves(cone, bounding_spheres, distance_field_data_blocks, num_distance_fields, cone_length);
}

#else

#if defined(BVH_WITH_STACK)
float coneSoftShadow(in Cone cone, in uint16_t* inner_nodes, in Sphere* bvh_inner_bounding_sphere, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  return coneSoftShadow_bvh_iterative(cone, uint16_t(0), inner_nodes, bvh_inner_bounding_sphere, bounding_spheres, distance_field_data_blocks, num_distance_fields, cone_length);
}
#endif

float coneSoftShadow(in Cone cone, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  return coneSoftShadow(cone, bvh_inner_nodes(), bvh_inner_bounding_spheres(), bounding_spheres, distance_field_data_blocks, num_distance_fields, cone_length);
}

#endif

float coneSoftShadow(in Cone cone, float cone_length=inf)
{
  return coneSoftShadow(cone, distance_fields_bounding_spheres(), distance_fields_voxelData(), distance_fields_num(), cone_length);
}

#endif // RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL
