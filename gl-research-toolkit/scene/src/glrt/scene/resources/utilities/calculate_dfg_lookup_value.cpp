#include <glrt/dependencies.h>
#include <glrt/glsl/math-cpp.h>

using namespace glrt::glsl;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::pow;
using glm::fract;

template<typename T>
T saturate(T value)
{
  return clamp<T>(value, 0., 1.);
}

#define in
#define out(x) x&

// Source: https://en.wikipedia.org/w/index.php?title=Low-discrepancy_sequence&oldid=731052708#Hammersley_set
vec2 Hammersley(uint n, uint N)
{
  auto g_2 = [N](uint n) {
    double sum = 0.;

    double base = 0.5;

    for(uint k=1; k<N; k<<=1)
    {
      if(n & k)
        sum += base;
      base *= 0.5;
    }

    return sum;
  };

  return vec2(g_2(n), double(n)/double(N));
}

vec2 getSample(uint i, uint sampleCount)
{
  return Hammersley(i, sampleCount);
}

#define ONLY_Fr_DisneyDiffuse
#include <pbs/brdf.glsl>
#include <ibl/ggx_importance/ImportanceSampleGGX.glsl>

void importanceSampleGGX_G(vec2 u, vec3 V, vec3 N, float roughness, float& NdotH, float& LdotH, vec3& L, float& G)
{
  L = ImportanceSampleGGX(u, roughness, N);

  vec3 H = normalize(V + L);
  NdotH  = saturate(dot(N, H));
  LdotH  = saturate(dot(L, H));

  G = D_GGX(NdotH, roughness);
}

#include <pbs/dfg-preintegration.glsl>

namespace glrt {
namespace scene {
namespace resources {
namespace utilities {

vec3 calculate_dfg_lookup_value(double u, double v)
{
  float roughness = float(v);
  float NoV = float(u);

  vec3 N = glm::vec3(0,0,1);
  vec3 V = glm::vec3(sin(acos(NoV)), 0.f, NoV);

  return integrateDFGOnly(V, N, roughness);
}

} // namespace glrt
} // namespace scene
} // namespace resources
} // namespace utilities
