#ifndef GLRT_FRAGMENTEDARRAY_H
#define GLRT_FRAGMENTEDARRAY_H

#include <glrt/toolkit/array.h>

namespace glrt {


template<typename T_value, typename T_handler>
struct FragmentedArray_Segment_Values
{
  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;
  typedef int segment_index;

  struct SegmentRanges
  {
  };
  typedef ArrayTraits_Unordered_POD<SegmentRanges> SegmentRanges_ArrayTraits;

  static void start_iterate(const T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    *index = 0;

    Q_UNUSED(data);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(ranges);
    Q_UNUSED(extra_data);
  }

  static void end_iterate(T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_UNUSED(data);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(ranges);
    Q_UNUSED(extra_data);
    Q_UNUSED(index);
  }

  static void iterate(T_value* data, int data_index, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_UNUSED(ranges);
    Q_UNUSED(index);
    Q_UNUSED(begin);
    Q_UNUSED(end);

    T_handler::handle_value(data, data_index, extra_data);
  }


  static void classify(const T_value* data, int begin, int end, SegmentRanges* ranges, extra_data_type extra_data)
  {
    Q_UNUSED(data);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(ranges);
    Q_UNUSED(extra_data);
  }

  static bool segmentLessThan(const T_value& a, const T_value& b)
  {
    return T_handler::valueLessThan(a, b);
  }

  static int update_region_to_update(int beginRegionToUpdate, int begin, int end, const T_value& value, const SegmentRanges& ranges)
  {
    Q_UNUSED(beginRegionToUpdate);
    Q_UNUSED(end);
    Q_UNUSED(value);
    Q_UNUSED(ranges);

    return begin;
  }

  static glm::ivec2 section_boundaries(int begin, int end, const SegmentRanges& ranges)
  {
    Q_UNUSED(ranges);
    return glm::ivec2(begin,end);
  }

  static glm::ivec2 section_boundaries_for_value(int begin, int end, const SegmentRanges& ranges, const T_value& value)
  {
    Q_UNUSED(ranges);
    Q_UNUSED(value);
    return glm::ivec2(begin,end);
  }
};


namespace implementation {

template<typename T_segment_type, typename T_segment_array_traits = typename DefaultTraits<T_segment_type>::type>
struct FragmentedArray_SegmentIndexTraits_VariableSegmentNumber_Generic
{
  template<class Base>
  struct SegmentRangesMixin : public Base
  {
    Array<T_segment_type, T_segment_array_traits> segment_value;

    SegmentRangesMixin()
    {
    }

    SegmentRangesMixin(const SegmentRangesMixin& other) = delete;
    SegmentRangesMixin&operator=(const SegmentRangesMixin& other) = delete;

    SegmentRangesMixin(SegmentRangesMixin&& other)
      : Base(std::move(other)),
        segment_value(std::move(other.segment_value))
    {
    }

    SegmentRangesMixin& operator=(SegmentRangesMixin&& other)
    {
      Base::operator=(std::move(other));
      segment_value.swap(other.segment_value);
      return *this;
    }

    int number_segments() const
    {
      Q_ASSERT(Base::number_segments() == segment_value.length());

      return Base::number_segments();
    }

    T_segment_type segment_value_for_index(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < number_segments());
      return segment_value[index];
    }

    int segment_as_index(T_segment_type segment) const
    {
      const int num_segments = number_segments();
      for(int i=0; i<num_segments; ++i)
      {
        if(segment_value_for_index(i) == segment)
          return i;
      }

      return -1;
    }

    void clear()
    {
      Base::clear();
      segment_value.clear();
    }

    void appendSegment(T_segment_type segment)
    {
      segment_value.append(segment);
    }

    void fillUpToSegment(T_segment_type segment, int end, bool goingToAppendEndAnyway)
    {
      Q_UNUSED(segment);
      Q_UNUSED(end);
      Q_UNUSED(goingToAppendEndAnyway);
    }
  };
};

template<class T_segment_type, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_SegmentIndexTraits_VariableSegmentNumber_IndexBased
{
  template<class Base>
  struct SegmentRangesMixin : public Base
  {
    SegmentRangesMixin()
    {
    }

    SegmentRangesMixin(const SegmentRangesMixin& other) = delete;
    SegmentRangesMixin&operator=(const SegmentRangesMixin& other) = delete;

    SegmentRangesMixin(SegmentRangesMixin&& other)
      : Base(std::move(other))
    {
    }

    SegmentRangesMixin& operator=(SegmentRangesMixin&& other)
    {
      Base::operator=(std::move(other));
      return *this;
    }

    int number_segments() const
    {
      return Base::number_segments();
    }


    T_segment_type segment_value_for_index(int index) const
    {
      Q_ASSERT(index >= 0);
      Q_ASSERT(index < number_segments());
      return T_handler::segment_from_index(index);
    }

    int segment_as_index(T_segment_type segment) const
    {
      int i = T_handler::segment_as_index(segment);

      return (i<0||i>=number_segments()) ? -1 : i;
    }

    void appendSegment(T_segment_type segment)
    {
      Q_UNUSED(segment);
    }

    void fillUpToSegment(T_segment_type segment, int end, bool goingToAppendEndAnyway)
    {
      int i = T_handler::segment_as_index(segment);

      if(goingToAppendEndAnyway)
        i--;

      while(i > Base::segmentEnd.length())
        Base::segmentEnd.append(end);
    }

  };
};

template<typename T_value, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_Base
{
  static bool segmentLessThan(const T_value& a, const T_value& b)
  {
    if(T_handler::segmentLessThan(a, b))
      return true;
    if(T_handler::segmentLessThan(b, a))
      return false;

    return T_inner_sections_trait::segmentLessThan(a, b);
  }
};


} // namespace implementation


template<typename T_value, typename T_segment_type, class T_handler, class T_inner_sections_trait, class T_segment_index_traits=implementation::FragmentedArray_SegmentIndexTraits_VariableSegmentNumber_Generic<T_segment_type>>
struct FragmentedArray_Segment_Generic : public implementation::FragmentedArray_Segment_Base<T_value, T_handler, T_inner_sections_trait>
{
  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;
  static_assert(std::is_same<typename T_handler::extra_data_type, typename T_inner_sections_trait::extra_data_type>::value, "Both extra_data_type, of the handler and the inner sections trait must be the same");

  struct segment_index
  {
    int index = 0;
    typename T_inner_sections_trait::segment_index inner_index;
  };

  struct SegmentRangesBase
  {
    Array<int> segmentEnd;
    Array<typename T_inner_sections_trait::SegmentRanges, ArrayTraits_Unordered_mCmOD<typename T_inner_sections_trait::SegmentRanges>> innerSegmentRanges;

    SegmentRangesBase(const SegmentRangesBase& other) = delete;
    SegmentRangesBase&operator=(const SegmentRangesBase& other) = delete;

    SegmentRangesBase()
    {
    }

    SegmentRangesBase(SegmentRangesBase&& other)
      : segmentEnd(std::move(other.segmentEnd)),
        innerSegmentRanges(std::move(other.innerSegmentRanges))
    {
    }

    SegmentRangesBase& operator=(SegmentRangesBase&& other)
    {
      segmentEnd.swap(other.segmentEnd);
      innerSegmentRanges.swap(other.innerSegmentRanges);
    }

    ~SegmentRangesBase()
    {
    }

    int number_segments() const
    {
      int nSegments = segmentEnd.length();

      Q_ASSERT(nSegments == segmentEnd.length());
      Q_ASSERT(nSegments == innerSegmentRanges.length());

      return nSegments;
    }

    int segment_start(int segment_index, int begin, int end) const
    {
      Q_ASSERT(segment_index >= 0);
      Q_ASSERT(segment_index < number_segments());
      Q_UNUSED(end);

      return segment_index==0 ?  begin : segmentEnd[segment_index-1];
    }

    int segment_end(int segment_index, int begin, int end) const
    {
      Q_ASSERT(segment_index >= 0);
      Q_ASSERT(segment_index < number_segments());
      Q_UNUSED(begin);
      Q_UNUSED(end);

      return segmentEnd[segment_index];
    }

    void clear()
    {
      innerSegmentRanges.clear();
      segmentEnd.clear();
    }

    void classify_subsegments(const T_value* data, extra_data_type extra_data)
    {
      const int n = segmentEnd.length();
      int subsegment_begin = 0;
      for(int i=0; i<n; ++i)
      {
        int subsegment_end = segmentEnd[i];

        innerSegmentRanges.append_move(std::move(typename T_inner_sections_trait::SegmentRanges()));
        T_inner_sections_trait::classify(data, subsegment_begin, subsegment_end, &innerSegmentRanges.last(), extra_data);

        subsegment_begin = subsegment_end;
      }
    }

    int firstNotEmptySubsegment(int begin, int end) const
    {
      Q_UNUSED(end);
      const int n = segmentEnd.length();
      for(int i=0; i<n; ++i)
        if(segmentEnd[i] != begin)
          return i;
      return -1;
    }

    bool isEmpty(int begin, int end) const
    {
      Q_UNUSED(end);
      return segmentEnd.isEmpty() || begin==segmentEnd.last();
    }
  };

  typedef typename T_segment_index_traits::template SegmentRangesMixin<SegmentRangesBase> SegmentRanges;


  static void start_iterate(T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_ASSERT(!ranges.isEmpty(begin, end));

    index->index = ranges.firstNotEmptySubsegment(begin, end);

    // If start_iterate gets called, we already know, that this segment is not empty. So there must be at least one not empty subsegment
    Q_ASSERT(index->index>=0 && index->index<ranges.number_segments());

    const int segment_start = ranges.segment_start(index->index, begin, end);
    const int segment_end = ranges.segment_end(index->index, begin, end);

    handler_type::handle_new_segment(data, segment_start, segment_end, ranges.segment_value_for_index(index->index), extra_data);
    T_inner_sections_trait::start_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
  }

  static void end_iterate(T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_ASSERT(!ranges.isEmpty(begin, end));

    const int num_segments = ranges.number_segments();
    Q_ASSERT(index->index < num_segments);

    const int segment_start = ranges.segment_start(index->index, begin, end);
    const int segment_end = ranges.segment_end(index->index, begin, end);

    T_inner_sections_trait::end_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
    handler_type::handle_end_segment(data, segment_start, segment_end, ranges.segment_value_for_index(index->index), extra_data);
  }

  static void iterate(T_value* data, int data_index, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_ASSERT(!ranges.isEmpty(begin, end));

    const int num_segments = ranges.number_segments();
    Q_ASSERT(index->index < num_segments);

    int segment_start = ranges.segment_start(index->index, begin, end);
    int segment_end = ranges.segment_end(index->index, begin, end);

    if(segment_end <= data_index)
    {
      T_inner_sections_trait::end_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
      handler_type::handle_end_segment(data, segment_start, segment_end, ranges.segment_value_for_index(index->index), extra_data);

      while((segment_end = ranges.segment_end(index->index, begin, end)) <= data_index)
      {
        index->index++;
        if(index->index >= num_segments)
          return;
      }

      segment_start = ranges.segment_start(index->index, begin, end);

      handler_type::handle_new_segment(data, segment_start, segment_end, ranges.segment_value_for_index(index->index), extra_data);
      T_inner_sections_trait::start_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
    }

    T_inner_sections_trait::iterate(data, data_index, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
  }

  static void classify(const T_value* data, int begin, int end, SegmentRanges* ranges, extra_data_type extra_data)
  {
    ranges->clear();

    if(begin == end)
      return;

    T_segment_type prevSegment = handler_type::classify(data[begin]);
    ranges->appendSegment(prevSegment);
    ranges->fillUpToSegment(prevSegment, begin, false);

    // #ISSUE-61 OMP  ??? calling T_inner_sections_trait::classify() might also useopen mp. measure whether it improves or damages performance. Maybe two functions classify and classify_parallel, the topmost trait gets called with _parallel it calls the inner traits without parallel (except FragmentedArray_Segment_SplitInTwo, this one calls both subtraits with _parallel)
    for(int i=begin+1; i<=end; ++i)
    {
      if(i!=end)
      {
        T_segment_type currentSegment = handler_type::classify(data[i]);
        if(currentSegment==prevSegment)
          continue;
        ranges->appendSegment(currentSegment);
        prevSegment = currentSegment;
        ranges->fillUpToSegment(currentSegment, i, true);
      }

      int subSegmentEnd = i;

      ranges->segmentEnd.append(subSegmentEnd);
    }

    ranges->classify_subsegments(data, extra_data);
  }

  static int update_region_to_update(int beginRegionToUpdate, int begin, int end, const T_value& value, const SegmentRanges& ranges)
  {
    int i = ranges.segment_as_index( handler_type::classify(value));

    if(i == -1)
      return beginRegionToUpdate;

    int segment_begin = ranges.segment_start(i, begin, end);
    int segment_end = ranges.segment_end(i, begin, end);
    return T_inner_sections_trait::update_region_to_update(beginRegionToUpdate, segment_begin, segment_end, value, ranges.innerSegmentRanges[i]);
  }

  template<typename... T_sub_segment_types>
  static glm::ivec2 section_boundaries(int begin, int end, const SegmentRanges& ranges, T_segment_type segment, T_sub_segment_types... sub_segments)
  {
    int i = ranges.segment_as_index(segment);

    if(i == -1)
      return glm::ivec2(-1);

    return T_inner_sections_trait::section_boundaries(ranges.segment_start(i, begin, end), ranges.segment_end(i, begin, end), ranges.innerSegmentRanges[i], sub_segments...);
  }

  static glm::ivec2 section_boundaries(int begin, int end, const SegmentRanges& ranges)
  {
    Q_UNUSED(ranges);
    return glm::ivec2(begin,end);
  }

  static glm::ivec2 section_boundaries_for_value(int begin, int end, const SegmentRanges& ranges, const T_value& value)
  {
    int i = ranges.segment_as_index(handler_type::classify(value));

    if(i == -1)
      return glm::ivec2(-1);

    return T_inner_sections_trait::section_boundaries_for_value(ranges.segment_start(i, begin, end), ranges.segment_end(i, begin, end), ranges.innerSegmentRanges[i], value);
  }
};

template<typename T_value, typename T_segment_type, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_Split_in_VariableNumber : public FragmentedArray_Segment_Generic<T_value, T_segment_type, T_handler, T_inner_sections_trait, implementation::FragmentedArray_SegmentIndexTraits_VariableSegmentNumber_IndexBased<T_segment_type, T_handler, T_inner_sections_trait>>
{
};

template<int N, typename T_value, typename T_segment_type, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_Split_in_FixedNumberOfSegments : public FragmentedArray_Segment_Split_in_VariableNumber<T_value, T_segment_type, T_handler, T_inner_sections_trait>
{
  // Dummy implementation for a possible future faster implementation of constant number of segments
};

template<typename T_value, typename T_segment_type, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_Split_in_TwoSegments : public FragmentedArray_Segment_Split_in_VariableNumber<T_value, T_segment_type, T_handler, T_inner_sections_trait>
{
  // Dummy implementation for a possible future faster implementation of two segments
};



template<typename T_data, typename T_sections_trait, typename T_data_array_traits = typename DefaultTraits<T_data>::type, typename T_data_array_allocator = DefaultAllocator<T_data>>
class FragmentedArray
{
public:
  typedef typename T_sections_trait::SegmentRanges SegmentRanges;
  typedef typename T_sections_trait::handler_type handler_type;
  typedef typename handler_type::extra_data_type extra_data_type;

  FragmentedArray();
  FragmentedArray(FragmentedArray&& other);
  FragmentedArray& operator=(FragmentedArray&& other);

  bool needsUpdate() const;
  T_data* data();
  const T_data* data() const;
  int length() const;

  void append_copy(const T_data& data);
  void append_move(T_data&& data);
  void remove(int);

  template<typename T_set>
  void removeMultiple(const T_set& s);

  int indexOf(const T_data& data) const;
  int indexOf_Safe(const T_data& data) const;
  void orderChangedForValue(const T_data& value);
  void orderChangedAtIndex(int index);

  int updateSegments(extra_data_type extra_data);
  void iterate(extra_data_type extra_data);

  template<typename... T_segment_types>
  glm::ivec2 section_boundaries(T_segment_types... segments) const;

private:
  SegmentRanges segmentRanges;
  Array<T_data, T_data_array_traits, T_data_array_allocator> dataArray;

  int beginRegionToUpdate = 0;

  void recalcRegionToUpdateAfterChanging(const T_data& data);
};


} // namespace glrt

#include "fragmented-array.inl"

#endif // GLRT_FRAGMENTEDARRAY_H
