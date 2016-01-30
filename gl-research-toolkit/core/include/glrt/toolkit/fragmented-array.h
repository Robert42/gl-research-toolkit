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

  static void init(SegmentRanges*){}

  static void start_iterate(const T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    *index = 0;

    Q_UNUSED(data);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(ranges);
    Q_UNUSED(extra_data);
  }

  static void end_iterate(const T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    Q_UNUSED(data);
    Q_UNUSED(begin);
    Q_UNUSED(end);
    Q_UNUSED(ranges);
    Q_UNUSED(extra_data);
    Q_UNUSED(index);
  }

  static void iterate(const T_value* data, int data_index, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
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


template<typename T_value, typename T_segment_type, class T_handler, class T_inner_sections_trait, typename T_segment_array_traits = typename DefaultTraits<T_segment_type>::type>
struct FragmentedArray_Segment_Generic : public FragmentedArray_Segment_Base<T_value, T_handler, T_inner_sections_trait>
{
  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;
  static_assert(std::is_same<typename T_handler::extra_data_type, typename T_inner_sections_trait::extra_data_type>::value, "Both extra_data_type, of the handler and the inner sections trait must be the same");

  struct segment_index
  {
    int index;
    typename T_inner_sections_trait::segment_index inner_index;
  };

  struct SegmentRanges
  {
    Array<T_segment_type, T_segment_array_traits> segment_value;
    Array<int> segmentEnd;
    Array<typename T_inner_sections_trait::SegmentRanges, ArrayTraits_Unordered_mCmOD<typename T_inner_sections_trait::SegmentRanges>> innerSegmentRanges;

    SegmentRanges(const SegmentRanges& other) = delete;
    SegmentRanges&operator=(const SegmentRanges& other) = delete;

    SegmentRanges()
    {
    }

    SegmentRanges(SegmentRanges&& other)
      : segment_value(std::move(other.segment_value)),
        segmentEnd(std::move(other.segmentEnd)),
        innerSegmentRanges(std::move(other.innerSegmentRanges))
    {
    }

    SegmentRanges& operator=(SegmentRanges&& other)
    {
      segment_value.swap(other.segment_value);
      segmentEnd.swap(other.segmentEnd);
      innerSegmentRanges.swap(other.innerSegmentRanges);
    }

    ~SegmentRanges()
    {
    }

    int number_segments() const
    {
      int nSegments = segment_value.length();

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
  };

  static void init(SegmentRanges*)
  {
  }

  static void start_iterate(const T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    if(ranges.segmentEnd.isEmpty())
      return;

    const int segment_start = ranges.segment_start(0, begin, end);
    const int segment_end = ranges.segment_end(0, begin, end);

    handler_type::handle_new_segment(data, segment_start, segment_end, ranges.segment_value[0], extra_data);

    index->index = 0;
    T_inner_sections_trait::start_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[0], extra_data, &index->inner_index);
  }

  static void end_iterate(const T_value* data, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    if(ranges.segmentEnd.isEmpty())
      return;

    const int num_segments = ranges.number_segments();
    Q_ASSERT(index->index < num_segments);

    const int segment_start = ranges.segment_start(index->index, begin, end);
    const int segment_end = ranges.segment_end(index->index, begin, end);

    T_inner_sections_trait::end_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
    handler_type::handle_end_segment(data, segment_start, segment_end, ranges.segment_value[index->index], extra_data);
  }

  static void iterate(const T_value* data, int data_index, int begin, int end, const SegmentRanges& ranges, extra_data_type extra_data, segment_index* index)
  {
    const int num_segments = ranges.number_segments();
    Q_ASSERT(index->index < num_segments);

    int segment_start = ranges.segment_start(index->index, begin, end);
    int segment_end = ranges.segment_end(index->index, begin, end);

    if(segment_end <= data_index)
    {
      T_inner_sections_trait::end_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
      handler_type::handle_end_segment(data, segment_start, segment_end, ranges.segment_value[index->index], extra_data);

      while((segment_end = ranges.segment_end(index->index, begin, end)) <= data_index)
      {
        index->index++;
        if(index->index >= num_segments)
          return;
      }

      segment_start = ranges.segment_start(index->index, begin, end);

      handler_type::handle_new_segment(data, segment_start, segment_end, ranges.segment_value[index->index], extra_data);
      T_inner_sections_trait::start_iterate(data, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
    }

    T_inner_sections_trait::iterate(data, data_index, segment_start, segment_end, ranges.innerSegmentRanges[index->index], extra_data, &index->inner_index);
  }

  static void classify(const T_value* data, int begin, int end, SegmentRanges* ranges, extra_data_type extra_data)
  {
    ranges->innerSegmentRanges.clear();
    ranges->segmentEnd.clear();
    ranges->segment_value.clear();

    if(begin == end)
      return;

    T_segment_type prevSegment = handler_type::classify(data[begin]);
    ranges->segment_value.append(prevSegment);
    int subSegmentStart = begin;

    // #ISSUE-61 OMP  ??? calling T_inner_sections_trait::classify() might also useopen mp. measure whether it improves or damages performance. Maybe two functions classify and classify_parallel, the topmost trait gets called with _parallel it calls the inner traits without parallel (except FragmentedArray_Segment_SplitInTwo, this one calls both subtraits with _parallel)
    for(int i=begin+1; i<=end; ++i)
    {
      if(i!=end)
      {
        T_segment_type currentSegment = handler_type::classify(data[i]);
        if(currentSegment==prevSegment)
          continue;
        ranges->segment_value.append(currentSegment);
        prevSegment = currentSegment;
      }

      int subSegmentEnd = i;

      ranges->segmentEnd.append(subSegmentEnd);
      ranges->innerSegmentRanges.append_move(std::move(typename T_inner_sections_trait::SegmentRanges()));
      T_inner_sections_trait::classify(data, subSegmentStart, subSegmentEnd, &ranges->innerSegmentRanges.last(), extra_data);
      subSegmentStart = subSegmentEnd;
    }
  }

  static int update_region_to_update(int beginRegionToUpdate, int begin, int end, const T_value& value, const SegmentRanges& ranges)
  {
    T_segment_type prevSegment = handler_type::classify(value);

    const int num_segments = ranges.number_segments();
    for(int i=0; i<num_segments; ++i)
    {
      int segment_begin = ranges.segment_start(i, begin, end);

      if(segment_begin > beginRegionToUpdate)
        return beginRegionToUpdate;

      if(ranges.segment_value[i] == prevSegment)
      {
        int segment_end = ranges.segment_end(i, begin, end);
        return T_inner_sections_trait::update_region_to_update(beginRegionToUpdate, segment_begin, segment_end, value, ranges.innerSegmentRanges[i]);
      }
    }

    return beginRegionToUpdate;
  }
};

// #TODO::::::::::::::::::::::::::: test FragmentedArray_Segment_Generic first, after success, also implement the two classes below

template<typename T_value, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_SplitInTwo
{
// #TODO
};

template<typename T_value, int N, class T_handler, class T_inner_sections_trait>
struct FragmentedArray_Segment_Split_in_N
{
  static_assert(N>=1, "N must be at last 1");

  // #TODO
};


template<typename T_data, typename T_sections_trait, typename T_data_array_traits = typename DefaultTraits<T_data>::type>
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
  void remove(const T_data& data);

  int updateSegments(extra_data_type extra_data);
  void iterate(extra_data_type extra_data);

private:
  SegmentRanges segmentRanges;
  Array<T_data, T_data_array_traits> dataArray;

  int beginRegionToUpdate = 0;

  void recalcRegionToUpdateAfterChanging(const T_data& data);
};


} // namespace glrt

#include "fragmented-array.inl"

#endif // GLRT_FRAGMENTEDARRAY_H
