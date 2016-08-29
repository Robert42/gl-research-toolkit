
Cone cone_bouquet[N_GI_CONES];
float cone_bouquet_ao[N_GI_CONES];
vec3 cone_normal;


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
    const float alpha = radians(60);
    const float c = cos(alpha);
    const float s = sin(alpha);
    const mat3 r = mat3( c, s, 0,
                        -s, c, 0,
                         0, 0, 1);
    for(int i=0; i<n; ++i)
    {
      cone_bouquet[i+n] = cone_bouquet[i];
      cone_bouquet[i+n].direction = r * cone_bouquet[i+n].direction;
    }
  }
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
