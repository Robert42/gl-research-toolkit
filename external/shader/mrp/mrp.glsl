vec3 getDirectionToLight(inout float evergyFactor, in Sphere sphere, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  // Equation 11
  vec3 L = sphere.origin - surface.position;
  vec3 r = surface.R;
  vec3 centerToRay = dot(L, r)*r - L;
  vec3 closesPoint = L + centerToRay * saturate(sphere.radius/length(centerToRay));
  float light_distance = length(closesPoint);
  vec3 l = closesPoint / light_distance;
  
  float alpha = surface.roughness;
  
  // Equation 10
  float alpha_ = saturate(alpha + sphere.radius / (2.f*light_distance));
  
  evergyFactor *= sq(alpha / alpha_);
  return l;
}

vec3 getDirectionToLight(inout float evergyFactor, in Tube tube, in SurfaceData surface) // using the center of the light as approximnation (see 4.7.4 for alternatives)
{
  evergyFactor *= 1.f;
  return normalize(tube.origin-surface.position);
}