#include <glrt/toolkit/fragmented-array.h>

namespace glrt {


template<typename d, typename s, typename t>
FragmentedArray<d, s, t>::FragmentedArray()
{
  s::init(&segmentRanges);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_copy(const d& data)
{
  dataArray.append_copy(data);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_move(d&& data)
{
  dataArray.append_move(std::move(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::remove(const d& data)
{
  dataArray.removeAt(dataArray.indexOf(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::updateSegments()
{
  dataArray.sort(handler_type::segmentLessThan);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::iterate(extra_data_type extra_data)
{
  int length = dataArray.length();

  s::start_iterate(length, segmentRanges, extra_data);

  for(int i=0; i<length; ++i)
  {
    s::iterate(i, length, segmentRanges, extra_data);
    handler_type::handle_value(dataArray.data(), i);
  }
}


} // namespace glrt
