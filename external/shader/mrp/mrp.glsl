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
  //return surface.R;
  // TODO try out specular dominant dir instead:
  return surface.dominant_specular_dir;
}

// TODO: improve performance by using a precomputed inverse influence_radius
float light_falloff(float distance_for_influence, float influence_radius, float distance_to_light)
{
  return sq(saturate(1.f - sq(sq(distance_for_influence/influence_radius)))) / (sq(distance_to_light) + 1.f);
}

vec3 _closestPointToSphere(in Sphere sphere, in vec3 reflection_direction)
{
  // Equation 11
  vec3 L = sphere.origin;
  vec3 r = reflection_direction;
  
  vec3 centerToRay = dot(L, r)*r - L;
  vec3 closesPoint = L + centerToRay * saturate(sphere.radius/length(centerToRay));
  
  return closesPoint;
}

vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Sphere sphere, in SurfaceData surface)
{
  // Equation 11
  sphere.origin -= surface.position;
  vec3 r = get_mrp_reflection_direction(surface);
  
  vec3 closesPoint = _closestPointToSphere(sphere, r);
  light_distance = length(closesPoint);
  vec3 l = closesPoint / light_distance;
  
  specularEnergyFactor = mrp_specular_correction_factor_area(sphere.radius, light_distance, surface);
  return l;
}

float _closestPointToLine_unclamped(vec3 start, vec3 startToEnd, float sqLineLength, in vec3 reflection_direction)
{
  vec3 L0 = start;
  vec3 Ld = startToEnd;
  vec3 r = reflection_direction;
  
  // FIXME: TODO decide
  // Equation 18
  return (dot(L0,Ld)*dot(r,L0) - dot(L0,L0)*dot(r,Ld)) / (dot(L0,Ld)*dot(r,Ld) - dot(Ld,Ld)*dot(r,L0));
  
  // Equation 19
  return (dot(r, start) * dot(r, Ld) - dot(L0, Ld)) / (sqLineLength - sq(dot(r, Ld)));
}

vec3 getDirectionToLight(out float specularEnergyFactor, out float light_distance, in Tube tube, in SurfaceData surface)
{
  // TODO
  specularEnergyFactor = 1.f;
  light_distance = length(tube.origin-surface.position);
  return (tube.origin-surface.position) / light_distance;
}