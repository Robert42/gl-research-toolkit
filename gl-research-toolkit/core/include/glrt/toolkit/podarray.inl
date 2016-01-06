#ifndef GLRT_PODARRAY_INL
#define GLRT_PODARRAY_INL

#include "podarray.h"

namespace glrt {

inline bool ranges_overlap(int range1_begin, int range1_end, int range2_begin, int range2_end)
{
  return (range1_begin>=range2_begin && range1_begin<range2_end)
      || (range2_begin>=range2_begin && range2_begin<range1_end)
      || (range1_begin>=range2_begin && range1_begin<range2_end)
      || (range2_begin>=range2_begin && range2_begin<range1_end);
}

template<typename T>
void DefaultTraits<T>::copy(T* dest, const T* src, int count)
{
  std::memmove(dest, src, sizeof(T)*count);
}

template<typename T>
void DefaultTraits<T>::copy_single(T* dest, const T* src)
{
  if(dest != src)
    new(dest)T(*src);
}

template<typename T>
int DefaultTraits<T>::new_capacity(int prev_capacity, int current_length, int elements_to_add, cache_type* cache)
{
  Q_UNUSED(cache);

  Q_ASSERT(prev_capacity >= 0);
  Q_ASSERT(current_length >= 0);
  Q_ASSERT(elements_to_add >= 0);

  return current_length + elements_to_add*4;
}

template<typename T>
int DefaultTraits<T>::adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed, cache_type* cache)
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
int DefaultTraits<T>::append(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  copy_single(data+prev_length, &value);
  return prev_length;
}

template<typename T>
int DefaultTraits<T>::extend(T* data, int prev_length, const T* values, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  copy(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T>
void DefaultTraits<T>::remove_single(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);

  copy_single(data+index, data+prev_length-1);
}

template<typename T>
void DefaultTraits<T>::remove(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
{
  Q_UNUSED(hint);
  Q_UNUSED(cache);
  Q_ASSERT(prev_length>=first_index+num_values);

  int first_value_to_keep = first_index+num_values;
  int first_value_to_copy;
  int num_values_to_copy;

  if(first_value_to_keep+num_values > prev_length)
  {
    first_value_to_copy = first_value_to_keep;
    num_values_to_copy = prev_length-first_value_to_keep;
  }else
  {
    num_values_to_copy = num_values;
    first_value_to_copy = prev_length-num_values_to_copy;
  }

  copy(data+first_index, data+first_value_to_copy, num_values_to_copy);
}


// =============================================================================


template<typename T, class T_traits>
PodArray<T, T_traits>::PodArray()
  : _data(nullptr),
    _capacity(0),
    _length(0)
{
  traits::init_cache(&this->trait_cache);
}

template<typename T, class T_traits>
PodArray<T, T_traits>::~PodArray()
{
  clear();
}

template<typename T, class T_traits>
PodArray<T, T_traits>::PodArray(PodArray&& other)
  : _data(other._data),
    _length(other._length)
{
  other._data = nullptr;
  other._length = nullptr;
  other._capacity = nullptr;
  traits::clear_cache(&other.trait_cache);
}

template<typename T, class T_traits>
PodArray<T, T_traits>& PodArray<T, T_traits>::operator=(PodArray<T, T_traits>&& other)
{
  this->swap(other);
  return *this;
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::swap(PodArray& other)
{
  std::swap(this->_data, other._data);
  std::swap(this->_capacity, other._capacity);
  std::swap(this->_length, other._length);
  traits::swap_cache(&this->trait_cache, other.trait_cache);
}


template<typename T, class T_traits>
int PodArray<T, T_traits>::capacity() const
{
  return _capacity;
}


template<typename T, class T_traits>
void PodArray<T, T_traits>::clear()
{
  _capacity = 0;
  _length = 0;
  delete[] _data;
  _data = nullptr;
  traits::clear_cache(&this->trait_cache);
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::setCapacity(int capacity)
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

      traits::copy(this->_data, old_data, this->_length);

      delete[] old_data;
    }
  }
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::ensureCapacity(int minCapacity)
{
  Q_ASSERT(capacity >= 0);

  setCapacity(glm::min(minCapacity, this->capacity()));
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::reserve(int minCapacity)
{
  Q_ASSERT(capacity >= 0);

  ensureCapacity(minCapacity);
}


template<typename T, class T_traits>
T* PodArray<T, T_traits>::data()
{
  return _data;
}

template<typename T, class T_traits>
const T* PodArray<T, T_traits>::data() const
{
  return _data;
}


template<typename T, class T_traits>
T& PodArray<T, T_traits>::at(int i)
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}

template<typename T, class T_traits>
const T& PodArray<T, T_traits>::at(int i) const
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}


template<typename T, class T_traits>
T& PodArray<T, T_traits>::operator[](int i)
{
  return at(i);
}

template<typename T, class T_traits>
const T& PodArray<T, T_traits>::operator[](int i) const
{
  return at(i);
}

template<typename T, class T_traits>
int PodArray<T, T_traits>::length() const
{
  return _length;
}

template<typename T, class T_traits>
int PodArray<T, T_traits>::append(const T& value, const hint_type& hint)
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
int PodArray<T, T_traits>::extend(const PodArray<T, T_other_traits>& values, const hint_type& hint)
{
  return extend(values.data(), values.length(), hint);
}

template<typename T, class T_traits>
int PodArray<T, T_traits>::extend(const T* values, int num_values, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values, &this->trait_cache));

  int new_index = traits::extend(this->data(), this->length(), values, num_values, hint, &this->trait_cache);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::remove(int index, const hint_type& hint)
{
  traits::remove_single(this->data(), this->length(), index, hint, &this->trait_cache);

  _length -= 1;

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), 1, &this->trait_cache);
  if(new_capacity >= 0)
    setCapacity(new_capacity);
}

template<typename T, class T_traits>
void PodArray<T, T_traits>::remove(int index, int num_to_remove, const hint_type& hint)
{
  traits::remove(this->data(), this->length(), index, num_to_remove, hint, &this->trait_cache);

  _length -= num_to_remove;

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), num_to_remove, &this->trait_cache);
  if(new_capacity >= 0)
    setCapacity(new_capacity);
}


} // namespace glrt


template<typename T, class T_traits>
void std::swap(glrt::PodArray<T, T_traits>& a, glrt::PodArray<T, T_traits>& b)
{
  a.swap(b);
}


#endif // GLRT_PODARRAY_INL
