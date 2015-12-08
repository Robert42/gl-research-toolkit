#ifdef FORWARD_RENDERER

#include <lighting/rendering-equation.glsl>

out vec4 color;

void apply_material(in MaterialOutput material_output, in vec3 direction_to_camera)
{
  ShadingInput shading_input;
  shading_input.surface_normal = material_output.normal;
  shading_input.surface_roughness = material_output.roughness;
  shading_input.surface_position = material_output.position;
  shading_input.direction_to_viewer = direction_to_camera;
  
  float alpha = material_output.color.a;
  vec3 emission = material_output.emission;
  vec3 diffuse_color = material_output.color.rgb;
  vec3 specular_color = mix(vec3(1), material_output.color.rgb, material_output.metallic);
  float specularfactor_dielectric = 0.05f;
  float specularfactor_metal = 1.f;
  float specularfactor = mix(specularfactor_dielectric, specularfactor_metal, material_output.metallic);
  
  ShadingOutput shading_output = direct_lighting(shading_input);
  
  vec3 lit_color = mix(diffuse_color*shading_output.diffuse_light, specular_color*shading_output.specular_light, specularfactor);
  
  color = vec4(emission + lit_color, alpha);
}

#endif // FORWARD_RENDERER
