#ifndef GLRT_CONCATENATEDLESSTHAN_INL
#define GLRT_CONCATENATEDLESSTHAN_INL

#include "concatenated-less-than.h"

namespace glrt {

template<typename T>
bool concatenated_lessThan(const T& a, const T& b)
{
  return a < b;
}

template<typename T, typename... T_rest>
bool concatenated_lessThan(const T& a, const T& b, const T_rest&... rest)
{
  if(concatenated_lessThan(a, b))
    return true;
  if(concatenated_lessThan(b, a))
    return false;

  return concatenated_lessThan(rest...);
}


} // namespace glrt

#endif // GLRT_CONCATENATEDLESSTHAN_INL
