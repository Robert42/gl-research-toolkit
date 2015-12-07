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
