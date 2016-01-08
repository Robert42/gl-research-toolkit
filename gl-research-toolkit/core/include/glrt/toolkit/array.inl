#ifndef GLRT_ARRAY_INL
#define GLRT_ARRAY_INL

#include "array.h"

namespace glrt {


/*! \class glrt::ArrayTraits_Unordered_Toolkit

Trait for working with unordered arrays. When removing a value from the array,
it may be swapped with the last value, so the index of a value might change.

The good news: indicers are guaranteed to be the same, as long as no items are
removed. Even if so, two different array, both with ArrayTraits_Unordered_Toolkit
traits will have the values consistently changed.
*/


template<typename T>
template<typename T_int>
inline bool ArrayTraits_Unordered_Toolkit<T>::ranges_overlap(T_int range1_begin, T_int range1_end, T_int range2_begin, T_int range2_end)
{
  return (range1_begin>=range2_begin && range1_begin<range2_end)
      || (range1_end  >=range2_begin && range1_end  <range2_end)
      || (range2_begin>=range1_begin && range2_begin<range1_end)
      || (range2_end  >=range1_begin && range2_end  <range1_end);
}

template<typename T>
inline bool ArrayTraits_Unordered_Toolkit<T>::ranges_overlap(const T* instanceA, const T* instanceB, int na, int nb)
{
  size_t a = size_t(instanceA);
  size_t b = size_t(instanceB);

  return ranges_overlap<size_t>(a, a+na*sizeof(T),
                                b, b+nb*sizeof(T));
}

template<typename T>
void ArrayTraits_Unordered_Toolkit<T>::swap_instances_mO(const T* a, const T* b, int n)
{
  Q_ASSERT(!ranges_overlap(a, b, n, n));

  for(int i=0; i<n; ++i)
    a[i] = std::move(b[i]);
}

template<typename T>
void ArrayTraits_Unordered_Toolkit<T>::call_instance_destructors_D(const T* a, int n)
{
  for(int i=0; i<n; ++i)
    (a+i)->~T();
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::values_used_to_fill_gaps(int* first, int* count, int prev_length, const int gap_start, int gap_length)
{
  Q_ASSERT(prev_length>=gap_start+gap_length);

  int& first_value_to_copy = *first;
  int& num_values_to_copy = *count;

  if(gap_start+gap_length+gap_length > prev_length)
  {
    first_value_to_copy = gap_start+gap_length;
    num_values_to_copy = prev_length-gap_start-gap_length;
  }else
  {
    num_values_to_copy = gap_length;
    first_value_to_copy = prev_length-num_values_to_copy;
  }

  Q_ASSERT(!ranges_overlap(first_value_to_copy, first_value_to_copy+num_values_to_copy,
                           gap_start, gap_start+gap_length));
}


template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::change_location_mI(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  std::memcpy(dest, src, sizeof(T)*count);
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::change_location_single_mI(T* dest, const T* src)
{
  change_location_mI(dest, src, 1);
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::change_location_single_cC(T* dest, const T* src)
{
  Q_ASSERT(!ranges_overlap(dest, src, 1));
  new(dest)T(*src);
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::change_location_cC(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  for(int i=0; i<count; ++i)
    change_location_single_cC(dest+i, src+i);
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::new_capacity(int prev_capacity, int current_length, int elements_to_add, cache_type* cache)
{
  Q_UNUSED(cache);

  Q_ASSERT(prev_capacity >= 0);
  Q_ASSERT(current_length >= 0);
  Q_ASSERT(elements_to_add >= 0);

  return current_length + elements_to_add*4;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed, cache_type* cache)
{
  Q_UNUSED(cache);

  Q_ASSERT(prev_capacity >= 0);
  Q_ASSERT(current_length >= 0);
  Q_ASSERT(elements_removed >= 0);

  if(elements_removed > current_length/2 || current_length*8<prev_capacity)
    return current_length;
  else
    return prev_capacity;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::append_mC(T* data, int prev_length, T&& value, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  new(data+prev_length)T(std::move(value));
  return prev_length;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::append_mI(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  change_location_single_mI(data+prev_length, &value);
  return prev_length;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::extend_mI(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  change_location_mI(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::append_cC(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  new(data+prev_length)T(value);
  return prev_length;
}

template<typename T>
inline int ArrayTraits_Unordered_Toolkit<T>::extend_cC(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  for(int i=0; i<num_values; ++i)
    new(data+i)T(values[i]);
  return prev_length;
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::remove_single_mI(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int last = prev_length-1;
  if(index != last)
    change_location_single_mI(data+index, data+last);
}

template<typename T>
void ArrayTraits_Unordered_Toolkit<T>::remove_mI(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  change_location_mI(data+first_index, data+first_value_to_copy, num_values_to_copy);
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::remove_single_mOD(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int last = prev_length-1;
  if(index != last)
    data[index] = std::move(data[last]);
  data[last].~T();
}

template<typename T>
void ArrayTraits_Unordered_Toolkit<T>::remove_mOD(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  swap_instances_mO(data+first_index, data+first_value_to_copy, num_values_to_copy);

  call_instance_destructors_D(data+prev_length-num_values_to_copy, num_values_to_copy);
}

template<typename T>
inline void ArrayTraits_Unordered_Toolkit<T>::remove_single_cCD(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int last = prev_length-1;
  data[index].~T();

  if(index != last)
  {
    new(data+index)T(data[last]);
    data[last].~T();
  }
}

template<typename T>
void ArrayTraits_Unordered_Toolkit<T>::remove_cCD(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  call_instance_destructors_D(data+first_index, num_values);
  change_location_cC(data+first_index, data+first_value_to_copy, num_values_to_copy);

  call_instance_destructors_D(data+prev_length-num_values_to_copy, num_values_to_copy);
}

// =============================================================================

/*! \class glrt::Array

An array optimized for performant storage of values.

\part Traits

The main difference between classical vector types and this array type is, that
the traits can influence the behavior better.

For example one trait could decide to keep an unordered array, so removing a value
can be done by replacing the removed value wit the last and decrease the array
length by one. This way removing elements from this array can have very fast algorithms.

Traits can also change the behavior in different ways, for example integers don't
need the destructor to be called when elements are removed and objects can be moved
by only moving the raw bytes of the buffer.
*/


template<typename T, class T_traits>
Array<T, T_traits>::Array()
  : _data(nullptr),
    _capacity(0),
    _length(0)
{
  traits::init_cache(&this->trait_cache);
}

template<typename T, class T_traits>
Array<T, T_traits>::~Array()
{
  _capacity = 0;
  _length = 0;
  delete[] _data;
  _data = nullptr;

  traits::delete_cache(&this->trait_cache);
}

template<typename T, class T_traits>
Array<T, T_traits>::Array(Array&& other)
  : _data(other._data),
    _length(other._length)
{
  other._data = nullptr;
  other._length = nullptr;
  other._capacity = nullptr;
  traits::clear_cache(&other.trait_cache);
}

template<typename T, class T_traits>
Array<T, T_traits>& Array<T, T_traits>::operator=(Array<T, T_traits>&& other)
{
  this->swap(other);
  return *this;
}

template<typename T, class T_traits>
void Array<T, T_traits>::swap(Array& other)
{
  std::swap(this->_data, other._data);
  std::swap(this->_capacity, other._capacity);
  std::swap(this->_length, other._length);
  traits::swap_cache(&this->trait_cache, other.trait_cache);
}


template<typename T, class T_traits>
int Array<T, T_traits>::capacity() const
{
  return _capacity;
}


template<typename T, class T_traits>
void Array<T, T_traits>::clear()
{
  _capacity = 0;
  _length = 0;
  delete[] _data;
  _data = nullptr;
  traits::clear_cache(&this->trait_cache);
}

template<typename T, class T_traits>
void Array<T, T_traits>::setCapacity(int capacity)
{
  Q_ASSERT(capacity >= 0);

  if(this->capacity() != capacity)
  {
    if(capacity == 0)
    {
      this->clear();
    }else
    {
      T* old_data = this->_data;

      this->_data = new T[capacity];
      this->_capacity = capacity;
      this->_length = glm::min(this->_length, capacity);

      traits::change_location(this->_data, old_data, this->_length);

      delete[] old_data;
    }
  }
}

template<typename T, class T_traits>
void Array<T, T_traits>::ensureCapacity(int minCapacity)
{
  Q_ASSERT(capacity >= 0);

  setCapacity(glm::min(minCapacity, this->capacity()));
}

template<typename T, class T_traits>
void Array<T, T_traits>::reserve(int minCapacity)
{
  Q_ASSERT(capacity >= 0);

  ensureCapacity(minCapacity);
}


template<typename T, class T_traits>
T* Array<T, T_traits>::data()
{
  return _data;
}

template<typename T, class T_traits>
const T* Array<T, T_traits>::data() const
{
  return _data;
}


template<typename T, class T_traits>
T& Array<T, T_traits>::at(int i)
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}

template<typename T, class T_traits>
const T& Array<T, T_traits>::at(int i) const
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}


template<typename T, class T_traits>
T& Array<T, T_traits>::operator[](int i)
{
  return at(i);
}

template<typename T, class T_traits>
const T& Array<T, T_traits>::operator[](int i) const
{
  return at(i);
}

template<typename T, class T_traits>
int Array<T, T_traits>::length() const
{
  return _length;
}

template<typename T, class T_traits>
int Array<T, T_traits>::append(T&& value, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1, &this->trait_cache));

  // the trait must assume, that there's enough space
  int new_index = traits::append_move(this->data(), this->length(), value, hint, &this->trait_cache);

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}

template<typename T, class T_traits>
int Array<T, T_traits>::append(const T& value, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1, &this->trait_cache));

  // the trait must assume, that there's enough space
  int new_index = traits::append(this->data(), this->length(), value, hint, &this->trait_cache);

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}

template<typename T, class T_traits>
template<typename T_other_traits>
int Array<T, T_traits>::extend(const Array<T, T_other_traits>& values, const hint_type& hint)
{
  return extend(values.data(), values.length(), hint);
}

template<typename T, class T_traits>
int Array<T, T_traits>::extend(const T* values, int num_values, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values, &this->trait_cache));

  int new_index = traits::extend(this->data(), this->length(), values, num_values, hint, &this->trait_cache);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits>
void Array<T, T_traits>::remove(int index, const hint_type& hint)
{
  traits::remove_single(this->data(), this->length(), index, hint, &this->trait_cache);

  _length -= 1;

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), 1, &this->trait_cache);
  if(new_capacity >= 0)
    setCapacity(new_capacity);
}

template<typename T, class T_traits>
void Array<T, T_traits>::remove(int index, int num_to_remove, const hint_type& hint)
{
  traits::remove(this->data(), this->length(), index, num_to_remove, hint, &this->trait_cache);

  _length -= num_to_remove;

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), num_to_remove, &this->trait_cache);
  if(new_capacity >= 0)
    setCapacity(new_capacity);
}


} // namespace glrt


template<typename T, class T_traits>
void std::swap(glrt::Array<T, T_traits>& a, glrt::Array<T, T_traits>& b)
{
  a.swap(b);
}


#endif // GLRT_ARRAY_INL
