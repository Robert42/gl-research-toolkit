#include "brdf.glsl"

struct DisneyBaseMaterial
{
  vec3 normal;
  vec3 baseColor;
  float smoothness;
  float metalMask;
  float reflectance;
};


void TODO_material(in DisneyBaseMaterial material)
{
  vec3 normal = material.normal;
  vec3 baseColor = material.baseColor;
  float smoothness = material.smoothness;
  float metalMask = material.metalMask;
  float reflectance = material.reflectance;

  float roughness = sq(1.f-smoothness);
  float f0 = 0.16f * sq(reflectance);
  
  // FIXME implement
}