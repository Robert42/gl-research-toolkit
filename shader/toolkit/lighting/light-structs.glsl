struct LightSource
{
  vec3 color;
  float luminance;
  vec3 origin;
  float influence_radius;
};


struct SphereAreaLight
{
  LightSource light;
  float radius;
};

struct RectAreaLight
{
  LightSource light;
  vec3 tangent1;
  float half_width;
  vec3 tangent2;
  float half_height;
};

