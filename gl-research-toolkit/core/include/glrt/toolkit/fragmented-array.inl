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
void FragmentedArray<d, s, t>::updateSegments(extra_data_type extra_data)
{
  // #TODO::::::::::::::::::::::::::: Keep track of which sections need to be updated, to recognize, when not-movable buffers have to be updated

  dataArray.sort(handler_type::segmentLessThan);

  const int length = dataArray.length();
  s::classify(0, length, &segmentRanges, extra_data);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::iterate(extra_data_type extra_data)
{
  const int length = dataArray.length();
  typename s::segment_index index;

  s::start_iterate(dataArray.data(), 0, 0, length, segmentRanges, extra_data, &index);

  for(int i=0; i<length; ++i)
  {
    s::iterate(dataArray.data(), i, 0, length, segmentRanges, extra_data, &index);
  }
}


} // namespace glrt
