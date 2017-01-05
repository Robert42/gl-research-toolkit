#version 450 core
#include "../debugging/posteffects/posteffect_ray.glsl"

void main()
{
  vec3 color = vec3(1,0.5,0);
  
#ifdef FORWARD_RENDERER
  color *= get_exposure();
  color = accurateLinearToSRGB(color);
#endif
  
  write_fragment_color(vec4(color, 1));
}
