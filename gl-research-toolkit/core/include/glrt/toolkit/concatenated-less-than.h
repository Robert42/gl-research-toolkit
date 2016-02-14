#ifndef GLRT_CONCATENATEDLESSTHAN_H
#define GLRT_CONCATENATEDLESSTHAN_H

namespace glrt {

template<typename T>
bool concatenated_lessThan(const T& a, const T& b);

template<typename T, typename... T_rest>
bool concatenated_lessThan(const T& a, const T& b, const T_rest&... rest);

} // namespace glrt

#include "concatenated-less-than.inl"

#endif // GLRT_CONCATENATEDLESSTHAN_H
