#ifndef GLRT_FRAGMENTEDARRAY_INL
#define GLRT_FRAGMENTEDARRAY_INL

#include <glrt/toolkit/fragmented-array.h>

namespace glrt {


template<typename d, typename s, typename t, typename a>
FragmentedArray<d, s, t, a>::FragmentedArray()
{
}


template<typename d, typename s, typename t, typename a>
FragmentedArray<d, s, t, a>::FragmentedArray(FragmentedArray&& other)
  : segmentRanges(std::move(other.segmentRanges)),
    dataArray(std::move(other.dataArray))
{
#ifdef QT_DEBUG
  this->_updated = other._updated;
  other._updated = true;
#endif
}


template<typename d, typename s, typename t, typename a>
FragmentedArray<d, s, t, a>& FragmentedArray<d, s, t, a>::operator=(FragmentedArray&& other)
{
  this->segmentRanges.swap(other.segmentRanges);
  this->dataArray.swap(other.dataArray);
#ifdef QT_DEBUG
  std::swap(this->_updated, other._updated);
#endif

  return *this;
}

template<typename d, typename s, typename t, typename a>
bool FragmentedArray<d, s, t, a>::needsUpdate() const
{
  return beginRegionToUpdate < this->length();
}

template<typename d, typename s, typename t, typename a>
d* FragmentedArray<d, s, t, a>::data()
{
  return dataArray.data();
}

template<typename d, typename s, typename t, typename a>
const d* FragmentedArray<d, s, t, a>::data() const
{
  return dataArray.data();
}

template<typename d, typename s, typename t, typename a>
int FragmentedArray<d, s, t, a>::length() const
{
  return dataArray.length();
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::append_copy(const d& data)
{
  recalcRegionToUpdateAfterChanging(data);

  dataArray.append_copy(data);
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::append_move(d&& data)
{
  recalcRegionToUpdateAfterChanging(data);

  dataArray.append_move(std::move(data));
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::remove(int index)
{
  Q_ASSERT(index>=0 && index<this->length());

  beginRegionToUpdate = glm::min<int>(index, beginRegionToUpdate);

  dataArray.removeAt(index);
}

template<typename d, typename s, typename t, typename a>
template<typename T_set>
void FragmentedArray<d, s, t, a>::removeMultiple(const T_set& set)
{
  int length = dataArray.length();
  d* const data = dataArray.data();

  int i=0;
  while(i<length)
  {
    if(set.contains(data[i]))
    {
      beginRegionToUpdate = glm::min<int>(i, beginRegionToUpdate);
      dataArray.removeAt(i);
      --length;
      break;
    }else
    {
      ++i;
    }
  }

  while(i<length)
  {
    if(set.contains(data[i]))
    {
      dataArray.removeAt(i);
      --length;
    }else
    {
      ++i;
    }
  }
}

/*!
\warning This implementation uses  the section_boundaries_for_value method of the given trait.
If the trait is using values of a object being destroyed right now, this might
become an issue. In this case, prefer indexOf_Safe, which is only using the
\l{Array::indexOfFirst}{indexOfFirst} method of the inner array.
\br
This might for example become an issue because of removing an object from the
fragmented_array after getting QObject::deleteLater signal. In this case the child
classes' destructors are already called and classify methids depending on values
of the child class are not usable anymore.
*/
template<typename d, typename s, typename t, typename a>
int FragmentedArray<d, s, t, a>::indexOf(const d& value) const
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

  return indexOf_Safe(value);
}

template<typename d, typename s, typename t, typename a>
int FragmentedArray<d, s, t, a>::indexOf_Safe(const d& value) const
{
  return dataArray.indexOfFirst(value);
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::orderChangedForValue(const d& value)
{
  orderChangedAtIndex(indexOf(value));
  recalcRegionToUpdateAfterChanging(value);
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::orderChangedAtIndex(int index)
{
  beginRegionToUpdate = glm::min<int>(beginRegionToUpdate, index);
}

template<typename d, typename s, typename t, typename a>
int FragmentedArray<d, s, t, a>::updateSegments(extra_data_type extra_data)
{
  int beginRegionToUpdate = this->beginRegionToUpdate;

  const int length = dataArray.length();
  d* data = dataArray.data();

  // #ISSUE-61 STL
  std::stable_sort(data+this->beginRegionToUpdate, data+length, s::segmentLessThan);

  Q_ASSERT(std::is_sorted(data, data+length, s::segmentLessThan));

  // #ISSUE-64: is it possible to reclassify only the changed part?
  s::classify(dataArray.data(), 0, length, &segmentRanges, extra_data);

  this->beginRegionToUpdate = length;
  return beginRegionToUpdate;
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::iterate(extra_data_type extra_data)
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

template<typename d, typename s, typename t, typename a>
template<typename... T_segment_types>
glm::ivec2 FragmentedArray<d, s, t, a>::section_boundaries(T_segment_types... segments) const
{
  const int length = this->length();
  return s::section_boundaries(0, length, segmentRanges, segments...);
}

template<typename d, typename s, typename t, typename a>
void FragmentedArray<d, s, t, a>::recalcRegionToUpdateAfterChanging(const d& data)
{
  const int length = dataArray.length();
  beginRegionToUpdate = glm::min<int>(beginRegionToUpdate, s::update_region_to_update(beginRegionToUpdate, 0, length, data, this->segmentRanges));
}


} // namespace glrt

#endif // GLRT_FRAGMENTEDARRAY_INL
