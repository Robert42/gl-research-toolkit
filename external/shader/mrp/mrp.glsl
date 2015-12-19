float mrp_specular_correction_factor_line(in float len, in float light_distance, in SurfaceData surface)
{
  float alpha = surface.roughness;
  // Equation 10
  float alpha_ = saturate(alpha + len / (2.f*light_distance));
  
  return alpha / alpha_;
}

float mrp_specular_correction_factor_area(in float radius, in float light_distance, in SurfaceData surface)
{
  return sq(mrp_specular_correction_factor_line(radius, light_distance, surface));
}

vec3 get_mrp_reflection_direction(in SurfaceData surface)
{
  return surface.R;
  // TODO try out specular dominant dir instead:
  //return surface.dominant_specular_dir;
}

vec3 getDirectionToLight(out float specularEnergyFactor, in Sphere sphere, in SurfaceData surface)
{
  // Equation 11
  vec3 L = sphere.origin - surface.position;
  vec3 r = get_mrp_reflection_direction(surface);
  vec3 centerToRay = dot(L, r)*r - L;
  vec3 closesPoint = L + centerToRay * saturate(sphere.radius/length(centerToRay));
  float light_distance = length(closesPoint);
  vec3 l = closesPoint / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(sphere.radius, light_distance, surface);
  return l;
}

vec3 getDirectionToLight(out float specularEnergyFactor, in Tube tube, in SurfaceData surface)
{
  // TODO
  specularEnergyFactor = 1.f;
  return normalize(tube.origin-surface.position);
}