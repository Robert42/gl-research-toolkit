#ifndef GLRT_BITMAGIC_INL
#define GLRT_BITMAGIC_INL

#include <glrt/toolkit/bit-magic.h>

namespace glrt {

template<typename T>
struct numberOfBits
{
  static const T value = sizeof(T) * 8;
};

// Equivalent to (value==0 ? 0 : 1)
template<typename T>
inline T one_if_not_zero(T value)
{
  // Only unsigned are supported, as future implementations might be using right
  // bitshift operators, which are implementation defined
  // see http://stackoverflow.com/a/4009922/2301866
  static_assert(!std::numeric_limits<T>::is_signed, "Only unsigned integers are supported here");

  // One day, replace this with an efficient bit operation without branching
  return (value==0 ? 0 : 1);
}


// Equivalent to (value==0 ? 1 : 0)
template<typename T>
inline T one_if_zero(T value)
{
  return 1 - one_if_not_zero(value);
}

// Warning: if value has more tha one bits set, the behavior is undefined
//
// returns i, if value==(1<<i)
template<typename T>
inline T bitIndexOf(T value)
{
  T index = 0;

  index |=  one_if_not_zero<T>(value & T(0xffffffff00000000)) << 5;
  index |=  one_if_not_zero<T>(value & T(0xffff0000ffff0000)) << 4;
  index |=  one_if_not_zero<T>(value & T(0xff00ff00ff00ff00)) << 3;
  index |=  one_if_not_zero<T>(value & T(0xf0f0f0f0f0f0f0f0)) << 2;
  index |=  one_if_not_zero<T>(value & T(0xcccccccccccccccc)) << 1;
  index |=  one_if_not_zero<T>(value & T(0xaaaaaaaaaaaaaaaa));

  return index;
}

} // namespace glrt

#endif // GLRT_BITMAGIC_INL
