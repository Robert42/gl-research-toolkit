#ifndef GLRT_FRAGMENTEDARRAY_INL
#define GLRT_FRAGMENTEDARRAY_INL

#include <glrt/toolkit/fragmented-array.h>

namespace glrt {


template<typename d, typename s, typename t>
FragmentedArray<d, s, t>::FragmentedArray()
{
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
bool FragmentedArray<d, s, t>::needsUpdate() const
{
  return beginRegionToUpdate < this->length();
}

template<typename d, typename s, typename t>
d* FragmentedArray<d, s, t>::data()
{
  return dataArray.data();
}

template<typename d, typename s, typename t>
const d* FragmentedArray<d, s, t>::data() const
{
  return dataArray.data();
}

template<typename d, typename s, typename t>
int FragmentedArray<d, s, t>::length() const
{
  return dataArray.length();
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_copy(const d& data)
{
  recalcRegionToUpdateAfterChanging(data);

  dataArray.append_copy(data);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::append_move(d&& data)
{
  recalcRegionToUpdateAfterChanging(data);

  dataArray.append_move(std::move(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::remove(const d& data)
{
  remove(indexOfFirst(data));
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::remove(int index)
{
  Q_ASSERT(index>=0 && index<this->length());

  beginRegionToUpdate = glm::min<int>(index, beginRegionToUpdate);

  dataArray.removeAt(index);
}

template<typename d, typename s, typename t>
int FragmentedArray<d, s, t>::indexOfFirst(const d& value) const
{
  const int length = this->length();
  glm::ivec2 bounds = s::section_boundaries_for_value(0, length, segmentRanges, value);

  if(bounds[0] < beginRegionToUpdate)
  {
    const d* data = this->data();
    for(int i=bounds[0]; i<bounds[1]; ++i)
      if(data[i] == value)
        return i;
  }

  return dataArray.indexOfFirst(value);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::orderChangedForValue(const d& value)
{
  beginRegionToUpdate = glm::min<int>(beginRegionToUpdate, indexOfFirst(value));
  recalcRegionToUpdateAfterChanging(value);
}

template<typename d, typename s, typename t>
int FragmentedArray<d, s, t>::updateSegments(extra_data_type extra_data)
{
  int beginRegionToUpdate = this->beginRegionToUpdate;

  const int length = dataArray.length();
  d* data = dataArray.data();

  // ISSUE-61 STL
  std::stable_sort(data+this->beginRegionToUpdate, data+length, s::segmentLessThan);

  Q_ASSERT(std::is_sorted(data, data+length, s::segmentLessThan));

  // #TODO: is it possible to reclassify only the changed part?
  s::classify(dataArray.data(), 0, length, &segmentRanges, extra_data);

  this->beginRegionToUpdate = length;
  return beginRegionToUpdate;
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::iterate(extra_data_type extra_data)
{
  const int length = dataArray.length();
  Q_ASSERT_X(!needsUpdate(), "FragmentedArray<>::iterate", "bug detected: trying to iterate over a not updated FragmetnedArray");

  if(segmentRanges.isEmpty(0, length))
    return;

  typename s::segment_index index;

  s::start_iterate(dataArray.data(), 0, length, segmentRanges, extra_data, &index);

  for(int i=0; i<length; ++i)
  {
    s::iterate(dataArray.data(), i, 0, length, segmentRanges, extra_data, &index);
  }

  s::end_iterate(dataArray.data(), 0, length, segmentRanges, extra_data, &index);
}

template<typename d, typename s, typename t>
template<typename... T_segment_types>
glm::ivec2 FragmentedArray<d, s, t>::section_boundaries(T_segment_types... segments) const
{
  const int length = this->length();
  return s::section_boundaries(0, length, segmentRanges, segments...);
}

template<typename d, typename s, typename t>
void FragmentedArray<d, s, t>::recalcRegionToUpdateAfterChanging(const d& data)
{
  const int length = dataArray.length();
  beginRegionToUpdate = glm::min<int>(beginRegionToUpdate, s::update_region_to_update(beginRegionToUpdate, 0, length, data, this->segmentRanges));
}


} // namespace glrt

#endif // GLRT_FRAGMENTEDARRAY_INL
