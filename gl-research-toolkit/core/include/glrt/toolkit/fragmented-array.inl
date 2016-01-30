#ifndef GLRT_FRAGMENTEDARRAY_INL
#define GLRT_FRAGMENTEDARRAY_INL

#include <glrt/toolkit/fragmented-array.h>

namespace glrt {


template<typename d, typename s, typename t>
FragmentedArray<d, s, t>::FragmentedArray()
{
  s::init(&segmentRanges);
}


template<typename d, typename s, typename t>
FragmentedArray<d, s, t>::FragmentedArray(FragmentedArray&& other)
  : segmentRanges(std::move(other.segmentRanges)),
    dataArray(std::move(other.dataArray))
{
#ifdef QT_DEBUG
  this->_updated = other._updated;
  other._updated = true;
#endif
}


template<typename d, typename s, typename t>
FragmentedArray<d, s, t>& FragmentedArray<d, s, t>::operator=(FragmentedArray&& other)
{
  this->segmentRanges.swap(other.segmentRanges);
  this->dataArray.swap(other.dataArray);
#ifdef QT_DEBUG
  std::swap(this->_updated, other._updated);
#endif

  return *this;
}


template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_copy(const d& data)
{
#ifdef QT_DEBUG
  _updated = false;
#endif
  dataArray.append_copy(data);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_move(d&& data)
{
#ifdef QT_DEBUG
  _updated = false;
#endif

  dataArray.append_move(std::move(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::remove(const d& data)
{
#ifdef QT_DEBUG
  _updated = false;
#endif

  dataArray.removeAt(dataArray.indexOf(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::updateSegments(extra_data_type extra_data)
{
  // #TODO::::::::::::::::::::::::::: Keep track of which sections need to be updated, to recognize, when not-movable buffers have to be updated

  dataArray.stable_sort(s::segmentLessThan);

  const int length = dataArray.length();
  s::classify(dataArray.data(), 0, length, &segmentRanges, extra_data);

#ifdef QT_DEBUG
  _updated = true;
#endif
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::iterate(extra_data_type extra_data)
{
#ifdef QT_DEBUG
  Q_ASSERT_X(_updated, "FragmentedArray<>::iterate", "bug detected: trying to iterate over a not updated FragmetnedArray");
#endif

  const int length = dataArray.length();
  typename s::segment_index index;

  s::start_iterate(dataArray.data(), 0, length, segmentRanges, extra_data, &index);

  for(int i=0; i<length; ++i)
  {
    s::iterate(dataArray.data(), i, 0, length, segmentRanges, extra_data, &index);
  }

  s::end_iterate(dataArray.data(), 0, length, segmentRanges, extra_data, &index);
}


} // namespace glrt

#endif // GLRT_FRAGMENTEDARRAY_INL
