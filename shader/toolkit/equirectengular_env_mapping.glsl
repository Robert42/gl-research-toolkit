#ifndef EQUIRECTANGULAR_ENV_MAPPING_GLSL
#define EQUIRECTANGULAR_ENV_MAPPING_GLSL

void viewdir_to_angles(in vec3 viewdir, out float pan, out float tilt)
{
  pan = atan(viewdir.y, viewdir.x);
  tilt = atan(viewdir.z, length(viewdir.xy));
}

vec2 viewdir_to_angles(in vec3 viewdir)
{
  float pan, tilt;
  viewdir_to_angles(viewdir, pan, tilt);
  return vec2(pan, tilt);
}

vec2 viewdir_to_uv_coord(in vec3 viewdir)
{
  vec2 angles = viewdir_to_angles(viewdir);
  
  angles.y *= 2.f;

  angles += pi;

  angles /= two_pi;

  return angles;
}

#endif
