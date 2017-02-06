#ifndef N_GI_CONES
#error N_GI_CONES undefined
#endif


Cone cone_bouquet[N_GI_CONES];
float cone_bouquet_ao[N_GI_CONES];
vec3 cone_normal;

#if 0
// Add the cos factor mentioned in the conetracing paper
const float cos_60_degreee = 0.50;
const float cos_22_5_degreee = 0.923879532511287f;

#if N_GI_CONES == 7 || N_GI_CONES == 14
#define APPLY_COS_FACTOR(x, i) mix(1.0, x, i<6 ? cos_60_degreee : 1.f)
#elif N_GI_CONES == 9 || N_GI_CONES == 18
#define APPLY_COS_FACTOR(x, i) mix(1.0, x, i<6 ? cos_60_degreee : cos_22_5_degreee)
#else
#error unknown num cones
#endif
#else
#define APPLY_COS_FACTOR(x, i) x
#endif

void init_cone_bouquet_ao()
{
  for(int i=0; i<N_GI_CONES; ++i)
    cone_bouquet_ao[i] = 1.f;
}

float accumulate_bouquet_to_total_occlusion()
{
  float V = 0.f;
  for(int i=0; i<N_GI_CONES; ++i)
    V += APPLY_COS_FACTOR(max(0, cone_bouquet_ao[i]), i);
  return V / N_GI_CONES;
}

float accumulate_bouquet_to_total_occlusion_unclamped()
{
  float V = 0.f;
  for(int i=0; i<N_GI_CONES; ++i)
    V += APPLY_COS_FACTOR(cone_bouquet_ao[i], i);
  return V / N_GI_CONES;
}


void SHOW_CONES()
{
  for(int i=0; i<N_GI_CONES; ++i)
    SHOW_VALUE(cone_bouquet[i]);
}

bool intersects_with_cone_bouquet(in Sphere sphere, float cone_length)
{
  vec3 cone_origin = cone_bouquet[0].origin;
  
  float distance_to_sphere = max(0, distance(cone_origin, sphere.origin) - sphere.radius);
  
  float side_of_sphere = dot(sphere.origin+cone_normal*sphere.radius - cone_origin, cone_normal);
  
  return distance_to_sphere <= cone_length &&  side_of_sphere >= 0;
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
  
#if N_GI_CONES == 7 || N_GI_CONES == 14
  cone_bouquet[6].origin = world_position;
  cone_bouquet[6].direction = vec3(0, 0, 1);
  cone_bouquet[6].tan_half_angle = tan_half_angle_of_60;
  cone_bouquet[6].inv_cos_half_angle = inv_cos_half_angle_of_60;
  
#elif N_GI_CONES == 9 || N_GI_CONES == 18
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

#if N_GI_CONES > 9
  {
    const int n = N_GI_CONES/2;
    
    const float alpha = radians(30);
    const float c = cos(alpha);
    const float s = sin(alpha);
    const mat3 r = mat3( c, s, 0,
                        -s, c, 0,
                         0, 0, 1);


    int i=n;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+1;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+2;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+3;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+4;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+5;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+6;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
#if N_GI_CONES == 18
    i = n+7;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
    i = n+8;
    cone_bouquet[i].direction = r * cone_bouquet[i%n].direction;
#endif
  }
#endif

  for(int i=0; i<N_GI_CONES; ++i)
  {
#if (CONE_BOUQUET_NOISE || defined(CONE_BOUQUET_UNDERWATER_CAUSICS)) && !defined(COMPUTE_SHADER)
#if defined(CONE_BOUQUET_UNDERWATER_CAUSICS)
    float alpha = scene.totalTime;
#elif defined(CONE_BOUQUET_NOISE)
    float alpha = snoise((gl_FragCoord.xy + vec2(scene.totalTime*1000, 0)));
#endif
    float c = cos(alpha);
    float s = sin(alpha);
    mat3 rot = mat3( c, s, 0,
                    -s, c, 0,
                     0, 0, 1);
#else
    mat3 rot = mat3(1);
#endif
    cone_bouquet[i].direction = tangent_to_worldspace * rot * cone_bouquet[i].direction;
  }
  
#if N_GI_CONES == 7 || N_GI_CONES == 14
  cone_normal = cone_bouquet[6].direction;
#elif N_GI_CONES == 9 || N_GI_CONES == 18
  cone_normal = tangent_to_worldspace * vec3(0,0,1);
#else
#error unexpected number of cones
#endif
}
