#ifdef FORWARD_RENDERER

vec3 calc_debug_lighting(vec3 world_pos, vec3 world_normal)
{
  int N = 4;
  vec3 directions[4];
  vec3 colors[4];
  
  directions[0] = vec3(-0.76, -0.27, -0.59);
  directions[1] = vec3(0, 0,-1);
  directions[2] = vec3(0, 0, 1);
  directions[3] = vec3(0.76, 0.63, 0.16);
  
  colors[0] = vec3(1, 0.7, 0.3);
  colors[1] = vec3(0.2, 0.3, 1.0)*0.5;
  colors[2] = vec3(1, 0.7, 0.5)*0.3;
  colors[3] = vec3(1)*0.3;
  
  vec3 result = vec3(0);
  for(int i=0; i<N; ++i)
    result += colors[i] * max(0, dot(world_normal, -directions[i]));
    return result;
}

vec3 direct_diffuse_light(vec3 world_pos, vec3 world_normal)
{
  return calc_debug_lighting(world_pos, world_normal);
}

#endif // FORWARD_RENDERER
