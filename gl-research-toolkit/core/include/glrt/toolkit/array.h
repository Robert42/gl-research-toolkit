#ifndef GLRT_ARRAY_H
#define GLRT_ARRAY_H

#include <glrt/dependencies.h>

namespace glrt {

// cC: copy constructor
// dC: default constructor
// mC: move constructor
// D: Destructor
// mI: movable Instance: just copy the bytes to a new location and don't use the
//                       old location anymore and you are fine. Examples are all
//                       primitive types, pod structs like glm::vec3, also
//                       gl wrapper like the buffer object of glhelper.
// aO: assignment Operator
// mO: move Operator

template<typename T>
struct ArrayTraits_Unordered_Toolkit
{
  typedef uint32_t hint_type;
  typedef uint32_t cache_type;

  static hint_type default_append_hint(){return hint_type(0xffffffff);}
  static hint_type default_remove_hint(){return hint_type(0xffffffff);}

  static void copy_mI(T* dest, const T* src, int count);
  static void copy_single_mI(T* dest, const T* src);
  static void copy_cC(T* dest, const T* src, int count);
  static void copy_single_cC(T* dest, const T* src);
  static int new_capacity(int prev_capacity, int current_length, int elements_to_add, cache_type* cache);
  static int adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed, cache_type* cache);

  static void init_cache(cache_type*){}
  static void clear_cache(cache_type*){}
  static void delete_cache(cache_type*){}
  static void swap_cache(cache_type*, cache_type*){}

  static int append_mC(T* data, int prev_length, T&& value, const hint_type& hint, cache_type* cache);
  static int append_mI(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache);
  static int extend_mI(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache);
  static int append_cC(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache);
  static int extend_cC(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache);

  static void remove_single_mI(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache);
  static void remove_mI(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache);
  static void remove_single_mOD(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache);
  static void remove_mOD(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache);
  static void remove_single_cCD(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache);
  static void remove_cCD(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache);

protected:
  template<typename T_int>
  static bool ranges_overlap(T_int range1_begin, T_int range1_end, T_int range2_begin, T_int range2_end);
  static bool ranges_overlap(const T* instanceA, const T* instanceB, int na=1, int nb=1);

  static void values_used_to_fill_gaps(int* first, int* count, int prev_length, const int gap_start, int num_values);

  static void swap_instances_mO(const T* a, const T* b, int n);
  static void call_instance_destructors_D(const T* a, int n);
};

template<typename T>
struct ArrayTraits_Unordered_mI : public ArrayTraits_Unordered_Toolkit<T>
{
  typedef ArrayTraits_Unordered_Toolkit<T> parent_type;
  typedef typename parent_type::hint_type hint_type;
  typedef typename parent_type::cache_type cache_type;

  static void copy(T* dest, const T* src, int count)
  {
    parent_type::copy_mI(dest, src, count);
  }

  static void copy_single(T* dest, const T* src)
  {
    parent_type::copy_single_mI(dest, src);
  }

  static int append_move(T* data, int prev_length, T&& value, const hint_type& hint, cache_type* cache)
  {
    return append(data, prev_length, value, hint, cache);
  }

  static int append(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache)
  {
    return parent_type::append_mI(data, prev_length, value, hint, cache);
  }

  static int extend(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache)
  {
    return parent_type::extend_mI(data, prev_length, values, num_values, hint, cache);
  }

  static void remove_single(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
  {
    parent_type::remove_single_mI(data, prev_length, index, hint, cache);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
  {
    parent_type::remove_mI(data, prev_length, first_index, num_values, hint, cache);
  }
};

// Thought for OpenGL Wrapper
template<typename T>
struct ArrayTraits_Unordered_mCmOmID : public ArrayTraits_Unordered_Toolkit<T>
{
  typedef ArrayTraits_Unordered_Toolkit<T> parent_type;
  typedef typename parent_type::hint_type hint_type;
  typedef typename parent_type::cache_type cache_type;

  static void copy(T* dest, const T* src, int count)
  {
    parent_type::copy_mI(dest, src, count);
  }

  static void copy_single(T* dest, const T* src)
  {
    parent_type::copy_single_mI(dest, src);
  }

  static int append_move(T* data, int prev_length, T&& value, const hint_type& hint, cache_type* cache)
  {
    return parent_type::append_mC(data, prev_length, std::move(value), hint, cache);
  }

  static int append(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache) = delete;
  static int extend(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache) = delete;

  static void remove_single(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
  {
    parent_type::remove_single_mOD(data, prev_length, index, hint, cache);
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
  {
    parent_type::remove_mOD(data, prev_length, first_index, num_values, hint, cache);
  }
};

template<typename T>
struct ArrayTraits_Unordered_POD : public ArrayTraits_Unordered_mI<T>
{
};

template<typename T>
struct DefaultTraits;

template<>
struct DefaultTraits<int>
{
  typedef ArrayTraits_Unordered_POD<int> type;
};


template<typename T, class T_traits = typename DefaultTraits<T>::type>
class Array final
{
public:
  typedef T_traits traits;
  typedef typename traits::hint_type hint_type;
  typedef typename traits::cache_type cache_type;

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

  T* data();
  const T* data() const;

  T& at(int i);
  const T& at(int i) const;

  T& operator[](int i);
  const T& operator[](int i) const;

  int length() const;
  bool isEmpty() const;

  int append(T&& value, const hint_type& hint=traits::default_append_hint());
  int append(const T& value, const hint_type& hint=traits::default_append_hint());
  template<typename T_other_trait>
  int extend(const Array<T, T_other_trait>& values, const hint_type& hint=traits::default_append_hint());
  int extend(const T* values, int num_values, const hint_type& hint=traits::default_append_hint());

  void remove(int index, const hint_type& hint=traits::default_remove_hint());
  void remove(int index, int num_to_remove, const hint_type& hint=traits::default_remove_hint());

  bool operator==(const Array& other) const;
  bool operator!=(const Array& other) const;

  QVector<T> toQVector() const;

private:
  cache_type trait_cache;
  T* _data;
  int _capacity;
  int _length;
};

template<typename T, typename T_traits>
QDebug operator<<(QDebug d, const Array<T, T_traits>& array);


} // namespace glrt

template<typename T>
void std::swap(glrt::Array<T>& a, glrt::Array<T>& b);

#include "array.inl"

#endif // GLRT_ARRAY_H
