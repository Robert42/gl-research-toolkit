vec3 material_brdf(in BrdfParameters brdf_param, in BaseMaterial material)
{
  
  
  vec3 f_s = brdf_specular(brdf_param, roughness, f0, f90);
  float f_d = brdf_diffuse(brdf_param, roughness);
  
  return f_s * specular_occlusion + f_d * diffuse_occlusion * diffuseColor;
}

vec3 material_brdf(in vec3 V, in vec3 L, in BaseMaterial material, in PrecomputedMaterial precomputedMaterial)
{
  return material_brdf(init_brdf_parameters(material.normal, V, L, precomputedMaterial), material);
}
