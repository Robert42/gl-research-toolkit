struct BaseMaterial
{
  vec3 normal;
  float smoothness;
  vec3 baseColor;
  float metalMask;
  vec3 emission;
  float reflectance;
  float occlusion;
};

// listing 26
float computeSpecOcclusion(float NdotV, float AO, float roughness)
{
    return saturate(pow(NdotV + AO, exp2(-16.0f * roughness - 1.0f)) - 1.0f + AO);
}

vec3 material_brdf(in BrdfParameters brdf_param, in BaseMaterial material)
{
  vec3 normal = material.normal;
  vec3 baseColor = material.baseColor;
  float smoothness = material.smoothness;
  float metalMask = material.metalMask;
  float reflectance = material.reflectance;
  float AO = material.occlusion;

  float roughness = sq(1.f-smoothness); // 3.2.1 & Figure 12
  vec3 f0 = mix(vec3(0.16f * sq(reflectance)), baseColor, metalMask); // 3.2.1 & Appendix D
  float f90 = saturate(50.0 * dot(f0, vec3(0.33))); // listing 27
  vec3 diffuseColor = mix(baseColor, vec3(0), metalMask); // Appendix D
  
  float NdotV = brdf_param.NdotV;
  
  float diffuse_occlusion = AO;
  float specular_occlusion = computeSpecOcclusion(NdotV, AO, roughness);
  
  return brdf_specular(brdf_param, roughness, f0, f90) * specular_occlusion + brdf_diffuse(brdf_param, roughness) * diffuse_occlusion;
}

vec3 material_brdf(in vec3 V, in vec3 L, in BaseMaterial material)
{
  // TODO: try out using dominant direction
  return material_brdf(init_brdf_parameters(material.normal, V, L), material);
}
