struct MaterialOutput
{
  vec4 color;
  vec3 emission;
  float roughness;
  vec3 position;
  float metallicness;
  vec3 normal;
  float occlusion;
};

out vec4 color;

void apply_material(in MaterialOutput material_output)
{
  color = vec4(material_output.emission, 0) + material_output.color;
}
