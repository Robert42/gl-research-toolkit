#ifndef GLRT_FRAGMENTEDARRAY_H
#define GLRT_FRAGMENTEDARRAY_H

#include <glrt/toolkit/array.h>

namespace glrt {


template<typename T_value, typename T_segment_type, class T_handler, typename T_segment_array_traits = typename DefaultTraits<T_segment_type>::type>
struct FragmentedArray_Segment_Generic
{
  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;

  struct SegmentRanges
  {
    Array<T_segment_type, T_segment_array_traits> segment_value;
    Array<int> length;
  };

  static void init(SegmentRanges*){}

  static void start_iterate(int, SegmentRanges, extra_data_type extra_data)
  {
    TODO
  }

  static void iterate(int current_index, int length, const SegmentRanges& ranges, extra_data_type extra_data)
  {
    Q_ASSERT(ranges.segment_value.length() == ranges.length.length());

    TODO
  }
};

template<typename T, class T_handler>
struct FragmentedArray_Segment_SplitInTwo
{
  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;
  typedef int SegmentRanges;

  static void init(SegmentRanges* firstSegmentLength)
  {
    *firstSegmentLength = 0;
  }

  static void start_iterate(int, SegmentRanges ranges, extra_data_type extra_data)
  {
    int current_segment = 0;
    handler_type::handle_new_segment(extra_data, current_segment, 0, ranges);
  }

  static void iterate(int current_index, int length, SegmentRanges ranges, extra_data_type extra_data)
  {
    if(ranges==current_index)
    {
      int current_segment = 1;
      handler_type::handle_new_segment(extra_data, current_segment, ranges, length);
    }
  }
};

template<typename T_value, int N, class T_handler>
struct FragmentedArray_Segment_Split_in_N
{
  static_assert(N>=1, "N must be at last 1");

  typedef T_handler handler_type;
  typedef typename T_handler::extra_data_type extra_data_type;
  typedef int SegmentRanges[N];

  static void init(SegmentRanges* ranges)
  {
    std::memset(ranges, 0, sizeof(int)*N);
  }

  static void start_iterate(int, const SegmentRanges& ranges, extra_data_type extra_data)
  {
    handler_type::handle_new_segment(extra_data, 0, ranges[0]);
  }

  static void iterate(int current_index, int length, const SegmentRanges& ranges, extra_data_type extra_data)
  {
    int current_segment = handler_type::current_segment(extra_data); // #TODO: is it possible to prevent this?

    int next_segment = current_segment+1;
    if(next_segment < N && ranges[next_segment]>=current_index)
      handler_type::handle_new_segment(extra_data, next_segment, ranges[next_segment], next_segment+1<N ? ranges[next_segment+1] : length);
  }
};

template<typename T_value, typename T1, typename T2>
struct FragmentedArray_Link
{
  typedef typename T1::handler_type handler_type1;
  typedef typename T2::handler_type handler_type2;
  typedef typename handler_type1::extra_data_type extra_data_type1;
  typedef typename handler_type2::extra_data_type extra_data_type2;

  static_assert(std::is_same<extra_data_type1, extra_data_type2>::value, "Both subtypes must have the same extra_type");
  typedef extra_data_type1 extra_data_type;

  struct handler_type
  {
    static bool segmentLessThan(const T_value& value1, const T_value& value2)
    {
      if(handler_type1::segmentLessThan(value1, value2))
        return true;
      if(handler_type1::segmentLessThan(value2, value1))
        return false;

      if(handler_type2::segmentLessThan(value1, value2))
        return true;
      if(handler_type2::segmentLessThan(value2, value1))
        return false;

      return false;
    }
  };

  struct SegmentRanges
  {
    typename T1::SegmentRanges ranges1;
    typename T2::SegmentRanges ranges2;
  };

  static void init(SegmentRanges* ranges)
  {
    T1::init(&ranges->ranges1);
    T2::init(&ranges->ranges2);
  }

  static void start_iterate(int length, const SegmentRanges& ranges, extra_data_type extra_data)
  {
    T1::iterate(length, ranges.ranges1, extra_data);
    T2::iterate(length, ranges.ranges2, extra_data);
  }

  static void iterate(int current_index, int length, const SegmentRanges& ranges, extra_data_type extra_data)
  {
    T1::iterate(current_index, length, ranges.ranges1, extra_data);
    T2::iterate(current_index, length, ranges.ranges2, extra_data);
  }
};


template<typename T_data, typename T_sections_trait, typename T_data_array_traits = typename DefaultTraits<T_data>::type>
class FragmentedArray
{
public:
  typedef typename T_sections_trait::SegmentRanges SegmentRanges;
  typedef typename T_sections_trait::handler_type handler_type;
  typedef typename handler_type::extra_data_type extra_data_type;

  SegmentRanges segmentRanges;
  Array<T_data, T_data_array_traits> dataArray;

  FragmentedArray();

  void append_copy(const T_data& data);
  void append_move(T_data&& data);
  void remove(const T_data& data);

  void updateSegments();
  void iterate(extra_data_type extra_data);
};


} // namespace glrt

#endif // GLRT_FRAGMENTEDARRAY_H
