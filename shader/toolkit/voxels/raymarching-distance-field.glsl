bool raymarch_distancefield(in Ray ray_worldspace, in VoxelData_AABB* voxelData, sampler3D* voxelTextures, uint32_t index, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
{
}

bool raymarch_distancefields(in Ray ray_worldspace, VoxelData_AABB* voxelData, sampler3D* voxelTextures, uint32_t num_voxels, float treshold, out vec3 intersection_point, out vec3 intersection_normal)
{
  float nearest_distance = inf;
  
  for(uint32_t i=0; i<num_voxels; ++i)
  {
    vec3 intersection_point_tmp;
    vec3 intersection_normal_tmp;
    
    bool got_hit = raymarch_distancefield(ray_worldspace, voxelData, voxelTextures, i, treshold, intersection_point_tmp, intersection_normal_tmp);

    float current_distance = sq_distance(intersection_point_tmp, ray_worldspace.origin);
    
    if(got_hit && current_distance < nearest_distance)
    {
        nearest_distance = current_distance;
        
        intersection_point = intersection_point_tmp;
        intersection_normal = intersection_normal_tmp;
    }
  }
  
  return !isinf(nearest_distance);
}