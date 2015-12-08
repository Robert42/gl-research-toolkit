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

vec3 direction_to_camera()
{
  return normalize(scene.view_position-fragment.position);
}
