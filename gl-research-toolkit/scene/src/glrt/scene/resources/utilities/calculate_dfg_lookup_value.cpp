#include <glrt/dependencies.h>
#include <glrt/glsl/math-cpp.h>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::pow;

#define saturate(value) clamp(value, 0.f, 1.f)

vec2 Hammersley(uint i, uint n);


#include <pbs/g_smith.glsl>
#include <ibl/lut/ImportanceSampleGGX.glsl>
#include <ibl/lut/IntegrateBRDF.glsl>

namespace glrt {
namespace scene {
namespace resources {
namespace utilities {

vec2 calculate_dfg_lookup_value(double u, double v)
{
  float roughness = float(u);
  float NoV = float(v);

  return IntegrateBRDF(roughness, NoV, glm::vec3(0,0,1));
}

} // namespace glrt
} // namespace scene
} // namespace resources
} // namespace utilities
