#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <gl-noise/src/noise2D.glsl>

#define N_GI_CONES 9

Cone cone_bouquet[N_GI_CONES];

int ao_distancefield_cost = 0;

float ao_coneSoftShadow(in Cone cone, in VoxelDataBlock* distance_field_data_block, float intersection_distance_front, float intersection_distance_back, float cone_length)
{
  mat4x3 worldToVoxelSpace;
  ivec3 voxelSize;
  vec3 voxelToUvwSpace;
  float worldToVoxelSpace_Factor;
  
  sampler3D texture = distance_field_data(distance_field_data_block, worldToVoxelSpace, voxelSize, voxelToUvwSpace, worldToVoxelSpace_Factor);
  
  #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
      ao_distancefield_cost++;
  #endif
  
  Ray ray_voxelspace = ray_world_to_voxelspace(ray_from_cone(cone), worldToVoxelSpace);
  
  float cone_length_voxelspace = cone_length * worldToVoxelSpace_Factor;
  float inv_cone_length_voxelspace = 1.f / cone_length_voxelspace;
  
  float self_shadow_avoidance = 0.25f; // TODO: use the distancefield itself to get the best offset? // TODO: use the id to deicide, whether the self occlusion offset should be applied?
  
  intersection_distance_front = max(intersection_distance_front*worldToVoxelSpace_Factor, self_shadow_avoidance);
  intersection_distance_back = min(intersection_distance_back*worldToVoxelSpace_Factor, cone_length_voxelspace);
    
  float minVisibility = 1.f;
  vec3 clamp_Range = vec3(voxelSize)-0.5f;
  
#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
  // In Range [intersection_distance_front, intersection_distance_back]
  float t = intersection_distance_front;
  
  int max_num_loops = 256;
  while(t < intersection_distance_back && 0<=max_num_loops--)
  {
#else
  // In Range [0, 1]
  float exponential = 1.f/16.f;
  
  for(int i=0; i<4; ++i)
  {
    float t = mix(intersection_distance_front, intersection_distance_back, exponential);
#endif
    vec3 p = get_point(ray_voxelspace, t);
    
    vec3 clamped_p = clamp(p, vec3(0.5), clamp_Range);
    
    float d = distancefield_distance(clamped_p, voxelToUvwSpace, texture) + distance(clamped_p, p);
#if defined(DISTANCEFIELD_AO_COST_TEX)
    ao_distancefield_cost++;
#endif
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone, t, d);
    occlusionHeuristic = mix(occlusionHeuristic, 1.f, t*inv_cone_length_voxelspace);
    minVisibility = min(minVisibility, occlusionHeuristic);
    
#if defined(DISTANCEFIELD_AO_SPHERE_TRACING)
    t += max(0.1f, abs(d));
#else
    exponential *= 2.f;
#endif
  }
  
  return minVisibility;
}

float ao_coneSoftShadow(in Cone cone, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  float occlusion = 1.f;
  
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
      occlusion = min(occlusion, ao_coneSoftShadow(cone, distance_field_data_blocks, intersection_distance_front, intersection_distance_back, cone_length));
    }
    
    ++bounding_spheres;
    ++distance_field_data_blocks;
  }
  
  occlusion = max(0, occlusion);
  
  return occlusion;
}

float distancefield_ao(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float radius=3.5)
{
  float V = 0.f;
  for(int i=0; i<N_GI_CONES; ++i)
  {
    V += ao_coneSoftShadow(cone_bouquet[i], bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  }
    
  return V / N_GI_CONES;
}

float distancefield_ao(float radius=3.5)
{
  Sphere* bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* distance_field_data_blocks = distance_fields_voxelData();
  uint32_t num_distance_fields = distance_fields_num();
  
  return distancefield_ao(bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
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
  {
#if defined(CONE_BOUQUET_NOISE) || defined(CONE_BOUQUET_UNDERWATER_CAUSICS)
#if defined(CONE_BOUQUET_UNDERWATER_CAUSICS)
    float alpha = scene.totalTime;
#elif defined(CONE_BOUQUET_NOISE)
    float alpha = snoise((gl_FragCoord.xy + vec2(scene.totalTime*1000, 0)));
#endif
    float c = cos(alpha);
    float s = sin(alpha);
    mat3 rot = mat3(c, -s, 0,
                    s,  c, 0,
                    0,  0, 1);
#else
    mat3 rot = mat3(1);
#endif
    cone_bouquet[i].direction = tangent_to_worldspace * rot * cone_bouquet[i].direction;
  }
}
