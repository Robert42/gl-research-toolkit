#ifndef GLRT_ZINDEX_H
#define GLRT_ZINDEX_H

#include <glrt/dependencies.h>

namespace glrt {

quint32 insertZeroBits(quint32 component);
quint32 calcZIndex(const glm::vec3& v);

} // namespace glrt

#endif // GLRT_ZINDEX_H
