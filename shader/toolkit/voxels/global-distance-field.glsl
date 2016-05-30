#include "distance-field-utils.glsl"

struct GlobalDistanceField
{
  uint32_t num_distance_fields;
  mat4* worldToVoxelSpaces;
  ivec3* voxelCounts;
  WorldVoxelUvwSpaceFactor* spaceFactors;
  sampler3D* distance_field_textures;
};

GlobalDistanceField init_global_distance_field()
{
  GlobalDistanceField gdf;
  gdf.num_distance_fields = distance_fields_num();
  gdf.worldToVoxelSpaces = distance_fields_worldToVoxelSpace();
  gdf.voxelCounts = distance_fields_voxelCount();
  gdf.spaceFactors = distance_fields_spaceFactor();
  gdf.distance_field_textures = distance_fields_texture();
  return gdf;
}

float distance_to_location(const in GlobalDistanceField globalDistanceField, vec3 location_ws, inout uint32_t stepCount)
{
  float d = inf;
  
  for(uint32_t i=0; i<globalDistanceField.num_distance_fields; ++i)
  {
    const vec3 location_voxelspace = transform_point(globalDistanceField.worldToVoxelSpaces[i], location_ws);
    const vec3 clamped_location_voxelspace = clamp(location_voxelspace, vec3(0.5), vec3(globalDistanceField.voxelCounts[i])-0.5f); // TODO: is it faster to use the texture size of distance_field_textures?
    
    WorldVoxelUvwSpaceFactor spaceFactor = globalDistanceField.spaceFactors[i];
    const float distance_within_voxel = distancefield_distance(location_voxelspace, spaceFactor, globalDistanceField.distance_field_textures[i]);
    const float distance_to_voxelgrid = spaceFactor.voxelToWorldSpace * distance(location_voxelspace, clamped_location_voxelspace);
    
    float voxelSpaceDistance = distance_within_voxel + distance_to_voxelgrid;
    float worldSpaceDistance = spaceFactor.voxelToWorldSpace * voxelSpaceDistance;
    
    d = min(d, worldSpaceDistance);
    stepCount++;
  }
  
  return d;
}

float distance_to_location(const in GlobalDistanceField globalDistanceField, vec3 location_ws, inout uint32_t stepCount, float max_radius)
{
  // TODO
  return distance_to_location(globalDistanceField, location_ws, stepCount);
}

bool raymarch(const in GlobalDistanceField globalDistanceField, in Ray ray, out vec3 world_pos, inout uint32_t stepCount)
{
  const float clip_front = 0.001f;
  const float clip_back = 50.f;
  
  float t = clip_front; 
  
  int max_num_loops = 1024;
  while(t < clip_back && 0<=max_num_loops--)
  {
    ++stepCount;
    
    vec3 p = get_point(ray, t);
    
    float d = distance_to_location(globalDistanceField, p, stepCount);
    
    if(d <= 1.e-2f)
    {
      world_pos = p;
      return true;
    }
    
    t += d;
  }
  
  return false;
}

vec3 distancefield_normal(const in GlobalDistanceField globalDistanceField, vec3 location_ws, inout uint32_t stepCount, float epsilon=0.01f)
{
  vec3 x1, x2, y1, y2, z1, z2;
  distancefield_normal_samplepoints(location_ws, x1, x2, y1, y2, z1, z2, epsilon=0.02f);
  
  return distancefield_normal(distance_to_location(globalDistanceField, x1, stepCount),
                              distance_to_location(globalDistanceField, x2, stepCount),
                              distance_to_location(globalDistanceField, y1, stepCount),
                              distance_to_location(globalDistanceField, y2, stepCount),
                              distance_to_location(globalDistanceField, z1, stepCount),
                              distance_to_location(globalDistanceField, z2, stepCount));
}

#define LINEAR_AO_STEPS 0

// Based on http://iquilezles.org/www/material/nvscene2008/rwwtt.pdf page 53
float distancefield_ambientocclusion(const in GlobalDistanceField globalDistanceField, vec3 location_ws, vec3 normal_ws, inout uint32_t stepCount)
{
  const float offset = 0;
  const float ao_radius = 3.5;
  const float max_query_radius = ao_radius * 0.5f;
  const int nQueries = 5;
  
  float samplingDistance = max_query_radius / 16.f;
  float delta = max_query_radius / nQueries;
  
  float attenuation = 0.5;
  
  float occlusion = 0.f;
  float maxDiff = 0.f;
    
  for(int i=1; i<=nQueries; ++i)
  {
#if LINEAR_AO_STEPS
    float expected_distance = offset + i * delta;
#else
    float expected_distance = offset + samplingDistance;
#endif
    float real_distance = distance_to_location(globalDistanceField, location_ws + normal_ws*expected_distance, stepCount);
    
    occlusion += attenuation * (expected_distance - real_distance);
    maxDiff += expected_distance;
    
    samplingDistance *= 2.f;
    attenuation *= 0.5f;
  }
  
  float strength = 4 / maxDiff;
  
  float ao = 1.f - occlusion * strength;
  
  return ao;
}
