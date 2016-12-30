
inline float G_Smith(float roughness, float NoV, float NoL)
{
  float alpha = roughness;

  // based on listing 3
  auto Lambda = [alpha](float theta_m)  -> float {
    float c = sq(theta_m);
    return (-1 + glm::sqrt(1.f + (sq(alpha) * (1.f - c)) / c)) / 2.f;
  };

  // \chi^+(v.h) \cdot \chi^+(h.h) test
  if(NoV <= 0.f || NoL<=0.f)
    return 0.f;

  return 1.f / (1.f + Lambda(NoV) + Lambda(NoL));
}
