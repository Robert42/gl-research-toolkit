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
  vec3 tangent1;
  float half_width;
  vec3 tangent2;
  float half_height;
  vec3 origin;
};

