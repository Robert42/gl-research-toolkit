#ifndef GLRT_PODARRAY_H
#define GLRT_PODARRAY_H

#include <glrt/dependencies.h>

namespace glrt {


template<typename T>
class DefaultTraits final
{
public:
  typedef uint32_t hint_type;
  typedef uint32_t cache_type;

  static const hint_type default_append_hint = hint_type(0xffffffff);
  static const hint_type default_remove_hint = hint_type(0xffffffff);

  static void copy(T* dest, const T* src, int count);
  static void copy_single(T* dest, const T* src);
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

template<typename T, class T_traits = DefaultTraits<T>>
class PodArray final
{
public:
  typedef T_traits traits;
  typedef typename traits::hint_type hint_type;
  typedef typename traits::cache_type cache_type;

  PodArray();
  ~PodArray();

  // Currently no need, uncomment if you need this
  PodArray(const PodArray&) = delete;
  PodArray& operator=(const PodArray&) = delete;

  PodArray(PodArray&& other);
  PodArray& operator=(PodArray&& other);

  void swap(PodArray& other);

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
  int extend(const PodArray<T, T_other_trait>& values, const hint_type& hint=traits::default_append_hint);
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
void std::swap(glrt::PodArray<T>& a, glrt::PodArray<T>& b);

#include "podarray.inl"

#endif // GLRT_PODARRAY_H
