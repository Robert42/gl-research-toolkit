#version 450 core
#include "../debugging/posteffects/posteffect_ray.glsl"

void main()
{
  Ray ray = view_ray();
  
  vec3 color = get_environment_infoming_ligth(ray.direction);
  
#if defined(FORWARD_RENDERER) && 0
  color *= get_exposure();
  color = accurateLinearToSRGB(color);
#endif
  
  write_fragment_color(vec4(color, 1));
}
