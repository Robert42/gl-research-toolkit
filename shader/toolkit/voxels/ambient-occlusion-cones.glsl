#include <voxels/raymarching-distance-cone-soft-shadow.glsl>
#include <gl-noise/src/noise2D.glsl>

#define N_GI_CONES 9

Cone cone_bouquet[N_GI_CONES];
float cone_bouquet_ao[N_GI_CONES];
vec3 cone_normal;

int ao_distancefield_cost = 0;

bool intersects_with_cone_bouquet(in Sphere sphere, float cone_length)
{
  vec3 cone_origin = cone_bouquet[0].origin;
  
  float distance_to_sphere = max(0, distance(cone_origin, sphere.origin) - sphere.radius);
  
  float side_of_sphere = dot(sphere.origin+cone_normal*sphere.radius - cone_origin, cone_normal);
  
  return distance_to_sphere <= cone_length &&  side_of_sphere >= 0;
}

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
    
    float cone_radius = cone.tan_half_angle * t;
    
    float occlusionHeuristic = coneOcclusionHeuristic(cone_radius, d);
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

void ao_coneSoftShadow_bruteforce(in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  for(uint32_t i=0; i<num_distance_fields; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    Sphere sphere = *bounding_spheres;
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      float distance_to_sphere_origin;
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length))
      {
        #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_coneSoftShadow(cone_bouquet[j], distance_field_data_blocks, intersection_distance_front, intersection_distance_back, cone_length));
      }
    }
    
    ++bounding_spheres;
    ++distance_field_data_blocks;
  }
}

void ao_coneSoftShadow_bvh(in Sphere* bvh_inner_bounding_sphere, uint16_t* inner_nodes, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float cone_length=inf)
{
  uint16_t stack[BVH_MAX_DEPTH];
  stack[0] = uint16_t(0);
  uint16_t stack_depth=uint16_t(1);

  uint16_t leaves[BVH_MAX_DEPTH];
  uint16_t num_leaves;
  num_leaves = uint16_t(0);

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
      if(intersects_with_cone_bouquet(bvh_inner_bounding_sphere[left_node], cone_length))
        stack[stack_depth++] = left_node;
    }else
    {
      #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
          ao_distancefield_cost++;
      #endif
      if(intersects_with_cone_bouquet(bounding_spheres[left_node], cone_length))
        leaves[num_leaves++] = left_node;
    }
      
    if(right_is_inner_node)
    {
      if(intersects_with_cone_bouquet(bvh_inner_bounding_sphere[right_node], cone_length))
        stack[stack_depth++] = right_node;
    }else
    {
      #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
          ao_distancefield_cost++;
      #endif
      if(intersects_with_cone_bouquet(bounding_spheres[right_node], cone_length))
        leaves[num_leaves++] = right_node;
    }
    
  }while(stack_depth>uint16_t(0));


  for(uint16_t i=uint16_t(0); i<num_leaves; ++i)
  {
    #if defined(DISTANCEFIELD_AO_COST_SDF_ARRAY_ACCESS)
        ao_distancefield_cost++;
    #endif
    Sphere sphere = bounding_spheres[leaves[i]];
    VoxelDataBlock* sdf = distance_field_data_blocks + leaves[i];
    
    for(int j=0; j<N_GI_CONES; ++j)
    {
      float distance_to_sphere_origin;
      if(cone_intersects_sphere(cone_bouquet[j], sphere, distance_to_sphere_origin, cone_length))
      {
        #if defined(DISTANCEFIELD_AO_COST_BRANCHING)
            ao_distancefield_cost++;
        #endif

        float intersection_distance_front = distance_to_sphere_origin-sphere.radius;
        float intersection_distance_back = distance_to_sphere_origin+sphere.radius;
        cone_bouquet_ao[j] = min(cone_bouquet_ao[j], ao_coneSoftShadow(cone_bouquet[j], sdf, intersection_distance_front, intersection_distance_back, cone_length));
      }
    }
  }
}

float distancefield_ao(in Sphere* bvh_bounding_spheres, uint16_t* bvh_nodes, in Sphere* bounding_spheres, in VoxelDataBlock* distance_field_data_blocks, uint32_t num_distance_fields, float radius=3.5)
{
  float V = 0.f;
  
  for(int i=0; i<N_GI_CONES; ++i)
    cone_bouquet_ao[i] = 1.f;
    
  #if defined(NO_BVH)
  ao_coneSoftShadow_bruteforce(bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #elif defined(BVH_WITH_STACK)
  ao_coneSoftShadow_bvh(bvh_bounding_spheres, bvh_nodes, bounding_spheres, distance_field_data_blocks, num_distance_fields, radius);
  #else
  #error UNKNOWN BVH usage
  #endif
    
  for(int i=0; i<N_GI_CONES; ++i)
    V += max(0, cone_bouquet_ao[i]);
    
  return V / N_GI_CONES;
}

float distancefield_ao(float radius=3.5)
{
  Sphere* _bvh_bounding_spheres = bvh_inner_bounding_spheres();
  uint16_t* _bvh_nodes = bvh_inner_nodes();
  Sphere* _bounding_spheres = distance_fields_bounding_spheres();
  VoxelDataBlock* _distance_field_data_blocks = distance_fields_voxelData();
  uint32_t _num_distance_fields = distance_fields_num();
  
  return distancefield_ao(_bvh_bounding_spheres, _bvh_nodes, _bounding_spheres, _distance_field_data_blocks, _num_distance_fields, radius);
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
  
#if N_GI_CONES == 7
  cone_normal = cone_bouquet[6].direction;
#elif N_GI_CONES == 9
  cone_normal = tangent_to_worldspace * vec3(0,0,1);
#else
#error unexpected number of cones
#endif
}
