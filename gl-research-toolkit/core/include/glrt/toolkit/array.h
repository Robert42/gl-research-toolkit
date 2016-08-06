#ifndef GLRT_ARRAY_H
#define GLRT_ARRAY_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/linked-tuple.h>

namespace glrt {

// cC: copy constructor
// dC: default constructor
// mC: move constructor
// D: Destructor
// POD: just copy the bytes to a new location and you are fine.
//      Use it for plain old data.
//      Examples are primitive types (bool, int, float),
//      pod structs like glm::vec3.
// aO: assignment Operator
// mO: move Operator

template<int block_size_append=128, int block_size_remove=512>
struct ArrayCapacityTraits_Capacity_Blocks
{
  static_assert(block_size_append>0, "The block_size_append must be > 0");
  static_assert(block_size_remove>0, "The block_size_remove must be > 0");

  static int new_capacity(int prev_capacity, int current_length, int elements_to_add);
  static int adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed);
  static int recalc_capacity(int prev_capacity, int current_length);
};

template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_Toolkit : public T_capacity_traits
{
  static void copy_construct_POD(T* dest, const T* src, int count);
  static void copy_construct_single_POD(T* dest, const T* src);
  static void copy_construct_cC(T* dest, const T* src, int count);
  static void copy_construct_single_cC(T* dest, const T* src);

  static void move_construct_mC(T* dest, T* src, int count);
  static void move_construct_single_mC(T* dest, T* src);

  static int append_mC(T* data, int prev_length, T&& value);
  static int extend_mC(T* data, int prev_length, T* values, int num_values);
  static int append_Primitive(T* data, int prev_length, T value);
  static int append_POD(T* data, int prev_length, const T& value);
  static int extend_POD(T* data, int prev_length, const T* values, int num_values);
  static int append_cC(T* data, int prev_length, const T& value);
  static int extend_cC(T* data, int prev_length, const T* values, int num_values);

  static void remove_single_Primitive(T* data, int prev_length, const int index);
  static void remove_single_POD(T* data, int prev_length, const int index);
  static void remove_POD(T* data, int prev_length, const int first_index, int num_values);
  static void remove_single_mOD(T* data, int prev_length, const int index);
  static void remove_mOD(T* data, int prev_length, const int first_index, int num_values);
  static void remove_single_aOD(T* data, int prev_length, const int index);
  static void remove_aOD(T* data, int prev_length, const int first_index, int num_values);
  static void remove_single_cCD(T* data, int prev_length, const int index);
  static void remove_cCD(T* data, int prev_length, const int first_index, int num_values);

  static void destruct_single_D(T* data);
  static void destruct_D(T* data, int length);

  template<typename T_int>
  static bool ranges_overlap(T_int range1_begin, T_int range1_end, T_int range2_begin, T_int range2_end);
  static bool ranges_overlap(const T* instanceA, const T* instanceB, int na=1, int nb=1);

  static void values_used_to_fill_gaps(int* first, int* count, int prev_length, const int gap_start, int num_values);

  static void swap_instances_mO(T* a, T* b, int n);
  static void swap_single_instance_mO(T* a, T* b);
  static void copy_instances_aO(T* a, T* b, int n);
  static void copy_single_instance_aO(T* a, T* b);
  static void call_instance_destructors_D(const T* a, int n);
};

// Primitives, like int or float
template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_Primitive : public ArrayTraits_Toolkit<T, T_capacity_traits>
{
  typedef ArrayTraits_Toolkit<T, T_capacity_traits> parent_type;

  static void move_construct(T* dest, T* src, int count)
  {
    parent_type::copy_construct_POD(dest, src, count);
  }

  static void move_construct_single(T* dest, T* src)
  {
    *dest = *src;
  }

  static int append_move(T* data, int prev_length, T value)
  {
    return append_copy(data, prev_length, value);
  }

  static int extend_move(T* data, int prev_length, const T* values, int num_values)
  {
    return extend_copy(data, prev_length, values, num_values);
  }

  static int append_copy(T* data, int prev_length, T value)
  {
    return parent_type::append_Primitive(data, prev_length, value);
  }

  static int extend_copy(T* data, int prev_length, const T* values, int num_values)
  {
    return parent_type::extend_POD(data, prev_length, values, num_values);
  }

  static void remove_single(T* data, int prev_length, const int index)
  {
    parent_type::remove_single_Primitive(data, prev_length, index);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values)
  {
    parent_type::remove_POD(data, prev_length, first_index, num_values);
  }

  static void destruct(T*, int)
  {
  }
};

// Plain old data, POD struct like vec3
template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_POD : public ArrayTraits_Toolkit<T, T_capacity_traits>
{
  typedef ArrayTraits_Toolkit<T, T_capacity_traits> parent_type;

  static void move_construct(T* dest, T* src, int count)
  {
    parent_type::copy_construct_POD(dest, src, count);
  }

  static void move_construct_single(T* dest, T* src)
  {
    parent_type::copy_construct_single_POD(dest, src);
  }

  static int append_move(T* data, int prev_length, T&& value)
  {
    return append_copy(data, prev_length, value);
  }

  static int extend_move(T* data, int prev_length, const T* values, int num_values)
  {
    return extend_copy(data, prev_length, values, num_values);
  }

  static int append_copy(T* data, int prev_length, const T& value)
  {
    return parent_type::append_POD(data, prev_length, value);
  }

  static int extend_copy(T* data, int prev_length, const T* values, int num_values)
  {
    return parent_type::extend_POD(data, prev_length, values, num_values);
  }

  static void remove_single(T* data, int prev_length, const int index)
  {
    parent_type::remove_single_POD(data, prev_length, index);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values)
  {
    parent_type::remove_POD(data, prev_length, first_index, num_values);
  }

  static void destruct(T*, int)
  {
  }
};

// Class with move constructor, move operator and destructor
// (Thought for OpenGL Wrapper)
template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_mCmOD : public ArrayTraits_Toolkit<T, T_capacity_traits>
{
  typedef ArrayTraits_Toolkit<T, T_capacity_traits> parent_type;

  static void move_construct(T* dest, T* src, int count)
  {
    parent_type::move_construct_mC(dest, src, count);
  }

  static void move_construct_single(T* dest, T* src)
  {
    parent_type::move_construct_single_mC(dest, src);
  }

  static int append_move(T* data, int prev_length, T&& value)
  {
    return parent_type::append_mC(data, prev_length, std::move(value));
  }

  static int extend_move(T* data, int prev_length, T* values, int num_values)
  {
    return parent_type::extend_mC(data, prev_length, values, num_values);
  }

  static int append_copy(T* data, int prev_length, const T& value) = delete;
  static int extend_copy(T* data, int prev_length, const T* values, int num_values) = delete;

  static void remove_single(T* data, int prev_length, const int index)
  {
    parent_type::remove_single_mOD(data, prev_length, index);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values)
  {
    parent_type::remove_mOD(data, prev_length, first_index, num_values);
  }

  static void destruct(T* data, int length)
  {
    parent_type::destruct_D(data, length);
  }
};

// Class with copy constructor, move constructor, move operator and destructor
template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_cCmCmOD : public ArrayTraits_mCmOD<T, T_capacity_traits>
{
  typedef ArrayTraits_mCmOD<T, T_capacity_traits> parent_type;

  static int append_copy(T* data, int prev_length, const T& value)
  {
    return parent_type::append_cC(data, prev_length, value);
  }

  static int extend_copy(T* data, int prev_length, const T* values, int num_values)
  {
    return parent_type::extend_cC(data, prev_length, values, num_values);
  }
};

// Class with copy constructor, assignment operator and destructor
template<typename T, class T_capacity_traits=ArrayCapacityTraits_Capacity_Blocks<>>
struct ArrayTraits_cCaOD : public ArrayTraits_Toolkit<T, T_capacity_traits>
{
  typedef ArrayTraits_Toolkit<T, T_capacity_traits> parent_type;

  static void move_construct(T* dest, T* src, int count)
  {
    parent_type::copy_construct_cC(dest, src, count);
  }

  static void move_construct_single(T* dest, T* src)
  {
    parent_type::copy_construct_single_cC(dest, src);
  }

  static int append_move(T* data, int prev_length, T&& value)
  {
    return parent_type::append_cC(data, prev_length, std::move(value));
  }

  static int extend_move(T* data, int prev_length, T* values, int num_values)
  {
    return parent_type::extend_cC(data, prev_length, values, num_values);
  }

  static int append_copy(T* data, int prev_length, const T& value)
  {
    return parent_type::append_cC(data, prev_length, value);
  }

  static int extend_copy(T* data, int prev_length, const T* values, int num_values)
  {
    return parent_type::extend_cC(data, prev_length, values, num_values);
  }

  static void remove_single(T* data, int prev_length, const int index)
  {
    parent_type::remove_single_aOD(data, prev_length, index);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values)
  {
    parent_type::remove_aOD(data, prev_length, first_index, num_values);
  }

  static void destruct(T* data, int length)
  {
    parent_type::destruct_D(data, length);
  }
};

template<typename T>
struct DefaultTraits;

template<>
struct DefaultTraits<qint8>
{
  typedef ArrayTraits_Primitive<qint32> type;
};

template<>
struct DefaultTraits<quint8>
{
  typedef ArrayTraits_Primitive<quint8> type;
};

template<>
struct DefaultTraits<qint16>
{
  typedef ArrayTraits_Primitive<qint16> type;
};

template<>
struct DefaultTraits<quint16>
{
  typedef ArrayTraits_Primitive<quint16> type;
};

template<>
struct DefaultTraits<qint32>
{
  typedef ArrayTraits_Primitive<qint32> type;
};

template<>
struct DefaultTraits<quint32>
{
  typedef ArrayTraits_Primitive<quint32> type;
};

template<>
struct DefaultTraits<qint64>
{
  typedef ArrayTraits_Primitive<qint64> type;
};

template<>
struct DefaultTraits<quint64>
{
  typedef ArrayTraits_Primitive<quint64> type;
};


template<>
struct DefaultTraits<bool>
{
  typedef ArrayTraits_Primitive<bool> type;
};

template<>
struct DefaultTraits<float>
{
  typedef ArrayTraits_Primitive<float> type;
};

template<typename T>
struct DefaultTraits<T*>
{
  typedef ArrayTraits_Primitive<T*> type;
};

template<typename T>
struct DefaultTraits<QPointer<T>>
{
  typedef ArrayTraits_cCaOD<QPointer<T>> type;
};

template<typename T>
struct DefaultAllocator
{
  static T* allocate_memory(int n);
  static void free_memory(T* data);
};

template<typename T, class T_prepended_type, int offset=sizeof(T_prepended_type)>
struct AllocatorWithPrependedData
{
  static_assert(offset>=sizeof(T_prepended_type), "offset must be at least as large as the prepended_type");
  typedef T_prepended_type prependet_type;

  static T* allocate_memory(int n);
  static void free_memory(T* data);

  static prependet_type& prepended_data(T* data);
  static const prependet_type& prepended_data(const T* data);

private:
  template<typename T_whole_buffer, typename T_data_buffer>
  static T_whole_buffer* whole_buffer(T_data_buffer* data_buffer);

  template<typename T_data_buffer, typename T_whole_buffer>
  static T_data_buffer* data_buffer(T_whole_buffer* whole_buffer);
};


template<typename T, class T_traits = typename DefaultTraits<T>::type, class T_allocator = DefaultAllocator<T>>
class Array final
{
public:
  typedef T_traits traits;
  typedef T_allocator allocator;

  Array();
  ~Array();

  // Currently no need, uncomment if you need this
  Array(const Array&) = delete;
  Array& operator=(const Array&) = delete;

  Array(const std::initializer_list<T>& init_with_values);
  Array(Array&& other);
  Array& operator=(Array&& other);

  void swap(Array& other);

  int capacity() const;
  void setCapacity(int capacity);
  void ensureCapacity(int minCapacity);
  void reserve(int minCapacity);
  void clear();

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;
  const T* constBegin() const;
  const T* constEnd() const;

  T* data();
  const T* data() const;

  T& at(int i);
  const T& at(int i) const;

  T& first(int i=0){return at(i);}
  const T& first(int i=0) const {return at(i);}
  T& last(int i=0){return at(length()-1-i);}
  const T& last(int i=0) const {return at(length()-1-i);}

  T& operator[](int i);
  const T& operator[](int i) const;

  int length() const;
  bool isEmpty() const;

  int append_move(T&& value);
  int extend_move(T* values, int num_values);

  int append_copy(const T& value);
  int extend_copy(const T* values, int num_values);

  int append(const T& value);
  int append(T&& value);

  void append_by_memcpy(const void* src, size_t num);
  void append_by_memcpy_items(int first, int num);
  template<typename T_value>
  void append_by_memcpy(const T_value& value);
  void resize(int newSize);

  void removeAt(int index);
  void removeAt(int index, int num_to_remove);

  void removeFirst(){removeAt(0);}
  void removeLast(){removeAt(length()-1);}

  int indexOfFirst(const T& value, int fallback=-1) const;

  bool operator==(const Array& other) const;
  bool operator!=(const Array& other) const;

  QVector<T> toQVector() const;

  template<typename T_lessThan>
  void stable_sort(T_lessThan lessThan);

  template<typename T_lessThan>
  void sort(T_lessThan lessThan);

private:
  T* _data;
  int _capacity;
  int _length;
};

template<typename T>
struct DefaultTraits<Array<T>>
{
  typedef ArrayTraits_mCmOD<Array<T>> type;
};

template<typename T, class T_traits, class T_allocator>
QDebug operator<<(QDebug d, const Array<T, T_traits, T_allocator>& array);


} // namespace glrt

namespace std {
template<typename T, class T_traits, class T_allocator>
void swap(glrt::Array<T, T_traits,T_allocator>& a, glrt::Array<T, T_traits,T_allocator>& b);
} // namespace std

#include "array.inl"

#endif // GLRT_ARRAY_H
