#ifndef GLRT_BITMAGIC_H
#define GLRT_BITMAGIC_H

#include <glrt/dependencies.h>

namespace glrt {

template<typename T>
struct numberOfBits;

template<typename T>
T one_if_not_zero(T value);

template<typename T>
T one_if_zero(T value);

template<typename T>
T bitIndexOf(T value);

} // namespace glrt

#include "bit-magic.inl"

#endif // GLRT_BITMAGIC_H
