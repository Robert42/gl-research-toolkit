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
class ArrayTraits_Unsorted_dCmImO
{
  typedef uint32_t hint_type;
  typedef uint32_t cache_type;

  static const hint_type default_append_hint = hint_type(0xffffffff);
  static const hint_type default_remove_hint = hint_type(0xffffffff);

  static void change_location(T* dest, const T* src, int count);
  static void change_location_single(T* dest, const T* src);
  static int new_capacity(int prev_capacity, int current_length, int elements_to_add, cache_type* cache);
  static int adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed, cache_type* cache);

  static void init_cache(cache_type*){}
  static void clear_cache(cache_type*){}
  static void swap_cache(cache_type*, cache_type*){}

  static int append(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache);
  static int extend(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache);
  static void remove_single(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache);
  static void remove(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache);
};

template<typename T>
class ArrayTraits_Unsorted_POD
{
};

template<typename T>
class ArrayTraits_Unsorted_cCdCmCDaOmO
{
public:
};

template<typename T>
struct DefaultTraits;

template<>
struct DefaultTraits<int>
{
  typedef ArrayTraits_Unsorted_POD<int> type;
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

  int append(const T& value, const hint_type& hint=traits::default_append_hint);
  template<typename T_other_trait>
  int extend(const Array<T, T_other_trait>& values, const hint_type& hint=traits::default_append_hint);
  int extend(const T* values, int num_values, const hint_type& hint=traits::default_append_hint);

  void remove(int index, const hint_type& hint=traits::default_remove_hint);
  void remove(int index, int num_to_remove, const hint_type& hint=traits::default_remove_hint);

private:
  cache_type trait_cache;
  T* _data;
  int _capacity;
  int _length;
};


} // namespace glrt

template<typename T>
void std::swap(glrt::Array<T>& a, glrt::Array<T>& b);

#include "array.inl"

#endif // GLRT_ARRAY_H
