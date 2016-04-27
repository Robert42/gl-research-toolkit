bool enter_voxel_grid(in Ray ray, in VoxelData_AABB aabb, out ivec3 voxelCoord, out int dimension)
{
  // #TODO
  return false;
}

bool next_grid(in Ray ray, in VoxelData_AABB aabb, out ivec3 voxelCoord, out int dimension)
{
  // #TODO
  return false;
}

vec3 cubic_voxel_surface_normal(in Ray ray, int hit_dimension)
{
  vec3 normal = vec3(0);
  
  normal[hit_dimension] = sign(-ray.direction[hit_dimension]);
  
  PRINT_VALUE(hit_dimension);
  
  return normal;
}