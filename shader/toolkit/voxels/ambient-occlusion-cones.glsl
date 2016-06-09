#include <voxels/raymarching-distance-cone-soft-shadow.glsl>

#define N_GI_CONES 9

Cone cone_bouquet[N_GI_CONES];

int ao_distancefield_cost = 0;

float ao_coneSoftShadow(in Cone cone, in mat4* worldToVoxelSpaces, in ivec3* voxelCounts, in WorldVoxelUvwSpaceFactor* spaceFactors, in sampler3D* distance_field_textures, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost+=2;
  #endif
  
  mat4 worldToVoxelSpace = *worldToVoxelSpaces;
  ivec3 voxelSize = *voxelCounts;
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  /* TODO check for performance boost
  float aabb_intersection_distance_front;
  float aabb_intersection_distance_back;
  if(intersects_aabb_twice(ray_voxelspace, vec3(0), vec3(voxelSize), aabb_intersection_distance_front, aabb_intersection_distance_back))
  {
    #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
        ao_distancefield_cost++;
    #endif
    intersection_distance_front = max(aabb_intersection_distance_front, intersection_distance_front);
    intersection_distance_back = min(aabb_intersection_distance_back, intersection_distance_back);
  }
  */
  
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost++;
  #endif
  
  WorldVoxelUvwSpaceFactor spaceFactor = *spaceFactors;
  float worldToVoxelSpace_Factor = 1.f / spaceFactor.voxelToWorldSpace;
  
  float cone_length_voxelspace = cone_length * worldToVoxelSpace_Factor;
  float inv_cone_length_voxelspace = 1.f / cone_length_voxelspace;
  
  float self_shadow_avoidance = 0.25f; // TODO: use the distancefield itself to get the best offset? // TODO: use the id to deicide, whether the self occlusion offset should be applied?
  
  intersection_distance_front = max(intersection_distance_front*worldToVoxelSpace_Factor, self_shadow_avoidance);
  intersection_distance_back = min(intersection_distance_back*worldToVoxelSpace_Factor, cone_length_voxelspace);
  
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost++;
  #endif
  
  sampler3D texture = *distance_field_textures;
    
  float t = intersection_distance_front;
  
  float minVisibility = 1.f;
  vec3 clamp_Range = vec3(voxelSize)-0.5f;
  
  int max_num_loops = 1024;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, spaceFactor, texture) + distance(clamped_p, p);
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone, t, d);
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, t*inv_cone_length_voxelspace);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
    t += abs(d);
  }
  
  return minVisibility;
}

float ao_coneSoftShadow(in Cone cone, in GlobalDistanceField global_distance_field, float cone_length=inf)
{
  uint32_t num_distance_fields = global_distance_field.num_distance_fields;
  
  float occlusion = 1.f;
  
  Sphere* bounding_spheres = global_distance_field.bounding_spheres;
  mat4* worldToVoxelSpaces = global_distance_field.worldToVoxelSpaces;
  ivec3* voxelCounts = global_distance_field.voxelCounts;
  WorldVoxelUvwSpaceFactor* spaceFactors = global_distance_field.spaceFactors;
  sampler3D* distance_field_textures = global_distance_field.distance_field_textures;
  
  for(uint32_t i=0; i<num_distance_fields; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    Sphere sphere = *bounding_spheres;
    
    float distance_to_sphere_origin;
    if(cone_intersects_sphere(cone, sphere, distance_to_sphere_origin, cone_length))
    {
      #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
          ao_distancefield_cost++;
      #endif

      float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
      float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
      occlusion = min(occlusion, ao_coneSoftShadow(cone, worldToVoxelSpaces, voxelCounts, spaceFactors, distance_field_textures, intersection_distance_front, intersection_distance_back, cone_length));
    }
    
    ++bounding_spheres;
    ++worldToVoxelSpaces;
    ++voxelCounts;
    ++spaceFactors;
    ++distance_field_textures;
  }
  
  occlusion = max(0, occlusion);
  
  return occlusion;
}

float distancefield_ao(in GlobalDistanceField global_distance_field, float radius=3.5)
{
  float V = 0.f;
  for(int i=0; i<N_GI_CONES; ++i)
  {
    V += ao_coneSoftShadow(cone_bouquet[i], global_distance_field, radius);
  }
    
  return V / N_GI_CONES;
}

float distancefield_ao(float radius=3.5)
{
  return distancefield_ao(init_global_distance_field(), radius);
}

void SHOW_CONES()
{
  for(int i=0; i<N_GI_CONES; ++i)
    SHOW_VALUE(cone_bouquet[i]);
}

void init_cone_bouquet(in mat3 tangent_to_worldspace, in vec3 world_position)
{
  const float tan_half_angle_of_60 = 0.577350269189626;
  const float tan_half_angle_of_45 = 0.414213562373095;
  const float inv_cos_half_angle_of_60 = 1.15470053837925;
  const float inv_cos_half_angle_of_45 = 1.08239220029239;
  
  cone_bouquet[0].origin = world_position;
  cone_bouquet[0].direction = vec3(0, -0.866025403784439, 0.5);
  cone_bouquet[0].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[0].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
  cone_bouquet[1].origin = world_position;
  cone_bouquet[1].direction = vec3(0.75, -0.433012701892219, 0.5);
  cone_bouquet[1].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[1].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
  cone_bouquet[2].origin = world_position;
  cone_bouquet[2].direction = vec3(0.75, 0.433012701892219, 0.5);
  cone_bouquet[2].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[2].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
  cone_bouquet[3].origin = world_position;
  cone_bouquet[3].direction = vec3(1.06057523872491e-16, 8.66025403784439e-01, 0.5);
  cone_bouquet[3].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[3].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
  cone_bouquet[4].origin = world_position;
  cone_bouquet[4].direction = vec3(-0.75, 0.433012701892220, 0.5);
  cone_bouquet[4].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[4].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
  cone_bouquet[5].origin = world_position;
  cone_bouquet[5].direction = vec3(-0.75, -0.433012701892219, 0.5);
  cone_bouquet[5].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[5].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
#if N_GI_CONES == 7
  cone_bouquet[6].origin = world_position;
  cone_bouquet[6].direction = vec3(0, 0, 1);
  cone_bouquet[6].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[6].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
#elif N_GI_CONES == 9
  cone_bouquet[6].origin = world_position;
  cone_bouquet[6].direction = vec3(0, -0.382683432365090, 0.923879532511287);
  cone_bouquet[6].tan_half_angle = tan_half_angle_of_45;
  cone_bouquet[6].inv_cos_half_angle = inv_cos_half_angle_of_45;
  
  cone_bouquet[7].origin = world_position;
  cone_bouquet[7].direction = vec3(0.331413574035592, 0.191341716182545, 0.923879532511287);
  cone_bouquet[7].tan_half_angle = tan_half_angle_of_45;
  cone_bouquet[7].inv_cos_half_angle = inv_cos_half_angle_of_45;
  
  cone_bouquet[8].origin = world_position;
  cone_bouquet[8].direction = vec3(-0.331413574035592, 0.191341716182545, 0.923879532511287);
  cone_bouquet[8].tan_half_angle = tan_half_angle_of_45;
  cone_bouquet[8].inv_cos_half_angle = inv_cos_half_angle_of_45;
#else
#error unexpected number of cones
#endif

  for(int i=0; i<N_GI_CONES; ++i)
    cone_bouquet[i].direction = tangent_to_worldspace * cone_bouquet[i].direction;
}
