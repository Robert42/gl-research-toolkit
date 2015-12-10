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
  float half_width;
  vec3 tangent;
  float half_height;
  vec3 origin;
};

