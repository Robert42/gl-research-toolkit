#include "global-distance-field.glsl"

#include <cone-tracing/cone-occlusion.glsl>

float coneSoftShadow(in Cone cone, uint32_t index, in GlobalDistanceField global_distance_field, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  mat4 worldToVoxelSpace = global_distance_field.worldToVoxelSpaces[index];
  ivec3 voxelSize = global_distance_field.voxelCounts[index];
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  /* TODO check for performance boost
  float aabb_intersection_distance_front;
  float aabb_intersection_distance_back;
  if(intersects_aabb_twice(ray_voxelspace, vec3(0), vec3(voxelSize), aabb_intersection_distance_front, aabb_intersection_distance_back))
  {
    intersection_distance_front = max(aabb_intersection_distance_front, intersection_distance_front);
    intersection_distance_back = min(aabb_intersection_distance_back, intersection_distance_back);
  }
  */
  
  WorldVoxelUvwSpaceFactor spaceFactor = global_distance_field.spaceFactors[index];
  float worldToVoxelSpace_Factor = 1.f / spaceFactor.voxelToWorldSpace;
  
  intersection_distance_front = max(intersection_distance_front, 0) * worldToVoxelSpace_Factor;
  intersection_distance_back = min(intersection_distance_back, cone_length) * worldToVoxelSpace_Factor;
  
  sampler3D texture = global_distance_field.distance_field_textures[index];
    
  float t = intersection_distance_front;
  
  float minVisibility = 1.f;
  
  int max_num_loops = 1024;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    float d = distancefield_distance(p, spaceFactor, texture);
    
    minVisibility = min(minVisibility, coneOcclusionHeuristic(cone, t, d));
    
    t += abs(d);
  }
  
  return minVisibility;
}

float coneSoftShadow(in Cone cone, in GlobalDistanceField global_distance_field, float cone_length=inf)
{
  uint32_t num_distance_fields = global_distance_field.num_distance_fields;
  
  float occlusion = 1.f;
  
  Sphere* bounding_spheres = global_distance_field.bounding_spheres;
  
  for(uint32_t i=0; i<num_distance_fields; ++i)
  {
    Sphere sphere = bounding_spheres[i];
    
    float distance_to_sphere_origin;
    if(cone_intersects_sphere(cone, sphere, distance_to_sphere_origin))
    {
      float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
      float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
      occlusion = min(occlusion, coneSoftShadow(cone, i, global_distance_field, intersection_distance_front, intersection_distance_back, cone_length));
    }
  }
  
  occlusion = max(0, occlusion);
  
  occlusion = sq(occlusion);
  
  return occlusion;
}

float coneSoftShadow(in Cone cone, float cone_length=inf)
{
  return coneSoftShadow(cone, init_global_distance_field(), cone_length);
}