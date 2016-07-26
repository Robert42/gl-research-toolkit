#include <glrt/toolkit/zindex.h>

namespace glrt {

quint32 insertZeroBits(quint32 component)
{
  quint32 z = 0;

  for(int i=0; i<10; ++i)
    z |= (component & (1<<i)) << (2*i);

  return z;
}

quint32 calcZIndex(const glm::vec3& v)
{
  glm::uvec3 uvec = glm::clamp(glm::uvec3(v * 1024.f), glm::uvec3(0), glm::uvec3(1023));

  return insertZeroBits(uvec.x) | (insertZeroBits(uvec.y)<<1) | (insertZeroBits(uvec.z)<<2);
}

} // namespace glrt
