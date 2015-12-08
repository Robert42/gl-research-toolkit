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


struct LightSource
{
  vec3 color;
  float luminance;
};


struct SphereAreaLight
{
  LightSource light;
  vec3 origin;
  float radius;
};

struct RectAreaLight
{
  LightSource light;
  vec3 normal;
  float halfWidth;
  vec3 tangent;
  float halfHeight;
  vec3 origin;
};

