struct ShadingInput
{
  vec3 surface_normal;
  float surface_roughness;
  vec3 surface_position;
  vec3 direction_to_viewer;
};

struct ShadingOutput
{
  vec3 diffuse_light;
  vec3 specular_light;
};