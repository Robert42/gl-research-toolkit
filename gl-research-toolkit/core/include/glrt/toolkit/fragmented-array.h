#ifndef GLRT_FRAGMENTEDARRAY_H
#define GLRT_FRAGMENTEDARRAY_H

#include <glrt/toolkit/array.h>

namespace glrt {

template<typename T_data, typename T_sections_trait, typename T_data_array_traits= typename DefaultTraits<T_data>::type>
class FragmentedArray
{
public:
  Array<T_data, T_data_array_traits> dataArray;

  FragmentedArray();
};

} // namespace glrt

#endif // GLRT_FRAGMENTEDARRAY_H
