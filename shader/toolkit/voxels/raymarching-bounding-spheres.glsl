#include "raymarching-cubic-voxels.glsl"
#include "distance-field-utils.glsl"

bool raymarch_boundingsphere_as_distancefield(in Ray ray_worldspace,
                                              in Sphere* spheres,
                                              in mat4* worldToVoxelSpaceMatrices,
                                              in WorldVoxelUvwSpaceFactor* spaceFactors,
                                              in ivec3* voxelSizes,
                                              uint32_t index,
                                              out float ray_hit_distance_worldspace,
                                              out vec3 intersection_normal_worldspace,
                                              inout uint32_t stepCount)
{
  mat4 worldToVoxelSpace = worldToVoxelSpaceMatrices[index];
  ivec3 voxelSize = voxelSizes[index];
  
  Sphere sphere = spheres[index];
  
  float intersection_distance_front;
  float intersection_distance_back;
  
#if 0
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_worldspace, worldToVoxelSpace);
  
  if(!intersects_aabb_twice(ray_voxelspace, vec3(0), vec3(voxelSize), intersection_distance_front, intersection_distance_back))
    return false;
    
  WorldVoxelUvwSpaceFactor spaceFactor = spaceFactors[index];
  
  intersection_distance_front = spaceFactor.voxelToWorldSpace * intersection_distance_front;
  intersection_distance_back = spaceFactor.voxelToWorldSpace * intersection_distance_back;
#else
  vec3 aabb_min = sphere.origin - vec3(sphere.radius * 2);
  vec3 aabb_max = sphere.origin + vec3(sphere.radius * 2);
  if(!intersects_aabb_twice(ray_worldspace, aabb_min, aabb_max, intersection_distance_front, intersection_distance_back))
    return false;
#endif
    
  float t = intersection_distance_front;
  
  int max_num_loops = 65536;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    ++stepCount;
    vec3 p = get_point(ray_worldspace, t);
    
    float d = distance(p, sphere.origin) - sphere.radius;
    
    if(d <= 1.e-2f)
    {
      ray_hit_distance_worldspace = t;
      intersection_normal_worldspace = normalize(p - sphere.origin);
      return true;
    }
    
    t += d;
  }
  
  return false;
}

bool raymarch_boundingspheres_as_distancefield(in Ray ray_worldspace,
                                               in Sphere* spheres,
                                               in mat4* worldToVoxelSpaceMatrices,
                                               in WorldVoxelUvwSpaceFactor* spaceFactors,
                                               in ivec3* voxelSizes,
                                               uint32_t num_voxels,
                                               out vec3 intersection_point,
                                               out vec3 intersection_normal,
                                               inout uint32_t stepCount)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    ++stepCount;
    
    float intersection_ray_distance_worldspace;
    vec3 intersection_normal_worldspace;
    
    bool got_hit = raymarch_boundingsphere_as_distancefield(ray_worldspace, spheres, worldToVoxelSpaceMatrices, spaceFactors, voxelSizes, i, intersection_ray_distance_worldspace, intersection_normal_worldspace, stepCount);

    float current_distance = intersection_ray_distance_worldspace;
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        intersection_normal = intersection_normal_worldspace;
    }
  }
  
  intersection_point = get_point(ray_worldspace, nearest_distance);
  
  return !isinf(nearest_distance);
}