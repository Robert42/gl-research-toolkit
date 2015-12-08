#include <preprocessor-block.h>

struct MaterialOutput
{
  vec4 color;
  vec3 emission;
  float roughness;
  vec3 position;
  float metallic;
  vec3 normal;
  float occlusion;
};

out vec4 color;

#ifdef FORWARD_RENDERER
#include "forward-lighting.fs.glsl"
#endif
