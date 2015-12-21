vec3 evaluate_specular_brdf_for_material(in BrdfData_Generic brdf_g, in BrdfData_WithLight brdf_l, in SurfaceData surface)
{
  vec3 f_s = brdf_specular(brdf_g, brdf_l, surface.f0, surface.f90);
  
  float specular_occlusion = surface.specular_occlusion;
  
  return f_s * specular_occlusion;
}

vec3 evaluate_diffuse_brdf_for_material(in BrdfData_Generic brdf_g, in BrdfData_WithLight brdf_l, in SurfaceData surface)
{
  float f_d = brdf_diffuse(brdf_g, brdf_l);
  
  float diffuse_occlusion = surface.diffuse_occlusion;
  vec3 diffuseColor = surface.diffuse_color;
  
  return f_d * diffuse_occlusion * diffuseColor;
}
