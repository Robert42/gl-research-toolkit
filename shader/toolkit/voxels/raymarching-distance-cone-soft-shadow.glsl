#ifndef RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL
#define RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL

#include <scene/uniforms.glsl>

#include "distance-field-utils.glsl"

#include <cone-tracing/cone-occlusion.glsl>

float coneSoftShadow(in Cone cone, in VoxelDataBlock* distance_field_data_block, float intersection_distance_front, float intersection_distance_back, float cone_length)
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

float coneSoftShadow(in Cone cone, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
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
      occlusion = min(occlusion, coneSoftShadow(cone, distance_field_data_blocks, intersection_distance_front, intersection_distance_back, cone_length));
    }
    
    ++bounding_spheres;
    ++distance_field_data_blocks;
  }
  
  occlusion = max(0, occlusion);
  
  occlusion = sq(occlusion);
  occlusion = smoothstep(0, 1, occlusion);
  
  return occlusion;
}

float coneSoftShadow(in Cone cone, float cone_length=inf)
{
  return coneSoftShadow(cone, distance_fields_bounding_spheres(), distance_fields_voxelData(), distance_fields_num(), cone_length);
}

#endif // RAYMARCHING_DISTANCEFIELD_CONE_SOFTSHADOW_GLSL