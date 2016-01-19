#ifndef GLRT_ARRAY_INL
#define GLRT_ARRAY_INL

#include "array.h"

namespace glrt {


template<int block_size_append, int block_size_remove>
inline int ArrayCapacityTraits_Capacity_Blocks<block_size_append, block_size_remove>::new_capacity(int prev_capacity, int current_length, int elements_to_add)
{
  Q_UNUSED(prev_capacity);

  Q_ASSERT(prev_capacity >= 0);
  Q_ASSERT(current_length >= 0);
  Q_ASSERT(elements_to_add >= 0);

  return glm::max(prev_capacity, glm::ceilMultiple(current_length+elements_to_add, block_size_append));
}

template<int block_size_append, int block_size_remove>
inline int ArrayCapacityTraits_Capacity_Blocks<block_size_append, block_size_remove>::adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed)
{
  Q_UNUSED(prev_capacity);

  Q_ASSERT(prev_capacity >= 0);
  Q_ASSERT(current_length >= 0);
  Q_ASSERT(elements_removed >= 0);

  return glm::min(prev_capacity, glm::ceilMultiple(current_length, block_size_remove));
}



/*! \class glrt::ArrayTraits_Unordered_Toolkit

Trait for working with unordered arrays. When removing a value from the array,
it may be swapped with the last value, so the index of a value might change.

The good news: indicers are guaranteed to be the same, as long as no items are
removed. Even if so, two different array, both with ArrayTraits_Unordered_Toolkit
traits will have the values consistently changed.

if removing one element, it is guaranted, that (if the removed element is not the
last one) the new index of the last value will be the previous index of the removed
value.
*/


template<typename T, typename T_c>
template<typename T_int>
inline bool ArrayTraits_Unordered_Toolkit<T, T_c>::ranges_overlap(T_int range1_begin, T_int range1_end, T_int range2_begin, T_int range2_end)
{
  return (range1_begin>=range2_begin && range1_begin<range2_end)
      || (range1_end  > range2_begin && range1_end  <range2_end)
      || (range2_begin>=range1_begin && range2_begin<range1_end)
      || (range2_end  > range1_begin && range2_end  <range1_end);
}

template<typename T, typename T_c>
inline bool ArrayTraits_Unordered_Toolkit<T, T_c>::ranges_overlap(const T* instanceA, const T* instanceB, int na, int nb)
{
  size_t a = size_t(instanceA);
  size_t b = size_t(instanceB);

  return ranges_overlap<size_t>(a, a+na*sizeof(T),
                                b, b+nb*sizeof(T));
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::swap_instances_mO(T* a, T* b, int n)
{
  Q_ASSERT(!ranges_overlap(a, b, n));

  for(int i=0; i<n; ++i)
    a[i] = std::move(b[i]);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::swap_single_instance_mO(T* a, T* b)
{
  Q_ASSERT(!ranges_overlap(a, b, 1));

  *a = std::move(*b);
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_instances_aO(T* a, T* b, int n)
{
  Q_ASSERT(!ranges_overlap(a, b, n));

  for(int i=0; i<n; ++i)
    a[i] = b[i];
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_single_instance_aO(T* a, T* b)
{
  Q_ASSERT(!ranges_overlap(a, b, 1));

  *a = *b;
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::call_instance_destructors_D(const T* a, int n)
{
  for(int i=0; i<n; ++i)
    (a+i)->~T();
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::values_used_to_fill_gaps(int* first, int* count, int prev_length, const int gap_start, int gap_length)
{
  Q_ASSERT(gap_start>=0);
  Q_ASSERT(gap_length>=0);
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


template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_construct_POD(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  std::memcpy(dest, src, sizeof(T)*count);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_construct_single_POD(T* dest, const T* src)
{
  copy_construct_POD(dest, src, 1);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_construct_single_cC(T* dest, const T* src)
{
  Q_ASSERT(!ranges_overlap(dest, src, 1));
  new(dest)T(*src);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::copy_construct_cC(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  for(int i=0; i<count; ++i)
    copy_construct_single_cC(dest+i, src+i);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::move_construct_mC(T* dest, T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  for(int i=0; i<count; ++i)
    move_construct_single_mC(dest+i, src+i);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::move_construct_single_mC(T* dest, T* src)
{
  Q_ASSERT(!ranges_overlap(dest, src, 1));
  new(dest)T(std::move(*src));
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::append_mC(T* data, int prev_length, T&& value)
{
  move_construct_single_mC(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::extend_mC(T* data, int prev_length, T* values, int num_values)
{
  move_construct_mC(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::append_POD(T* data, int prev_length, const T& value)
{
  copy_construct_single_POD(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::extend_POD(T* data, int prev_length, const T* values, int num_values)
{
  copy_construct_POD(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::append_cC(T* data, int prev_length, const T& value)
{
  copy_construct_single_cC(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Unordered_Toolkit<T, T_c>::extend_cC(T* data, int prev_length, const T* values, int num_values)
{
  copy_construct_cC(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_single_POD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    copy_construct_single_POD(data+index, data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_POD(T* data, int prev_length, const int first_index, int num_values)
{
  Q_ASSERT(first_index>=0);
  Q_ASSERT(first_index<prev_length);
  Q_ASSERT(first_index+num_values>=0);
  Q_ASSERT(first_index+num_values<=prev_length);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  copy_construct_POD(data+first_index, data+first_value_to_copy, num_values_to_copy);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_single_mOD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    swap_single_instance_mO(data+index, data + last);
  destruct_single_D(data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_mOD(T* data, int prev_length, const int first_index, int num_values)
{
  Q_ASSERT(first_index>=0);
  Q_ASSERT(first_index<prev_length);
  Q_ASSERT(first_index+num_values>=0);
  Q_ASSERT(first_index+num_values<=prev_length);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  swap_instances_mO(data+first_index, data+first_value_to_copy, num_values_to_copy);

  call_instance_destructors_D(data+prev_length-num_values, num_values);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_single_aOD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    copy_single_instance_aO(data+index, data + last);
  destruct_single_D(data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_aOD(T* data, int prev_length, const int first_index, int num_values)
{
  Q_ASSERT(first_index>=0);
  Q_ASSERT(first_index<prev_length);
  Q_ASSERT(first_index+num_values>=0);
  Q_ASSERT(first_index+num_values<=prev_length);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  copy_instances_aO(data+first_index, data+first_value_to_copy, num_values_to_copy);

  call_instance_destructors_D(data+prev_length-num_values, num_values);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_single_cCD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  destruct_single_D(data+index);

  if(index != last)
    copy_construct_single_cC(data+index, data+last);
  destruct_single_D(data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Unordered_Toolkit<T, T_c>::remove_cCD(T* data, int prev_length, const int first_index, int num_values)
{
  Q_ASSERT(first_index>=0);
  Q_ASSERT(first_index<prev_length);
  Q_ASSERT(first_index+num_values>=0);
  Q_ASSERT(first_index+num_values<=prev_length);

  int first_value_to_copy;
  int num_values_to_copy;

  values_used_to_fill_gaps(&first_value_to_copy, &num_values_to_copy, prev_length, first_index, num_values);

  call_instance_destructors_D(data+first_index, num_values);
  copy_construct_cC(data+first_index, data+first_value_to_copy, num_values_to_copy);

  call_instance_destructors_D(data+prev_length-num_values_to_copy, num_values_to_copy);
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::destruct_single_D(T* data)
{
  Q_ASSERT(data!=nullptr);

  (data)->~T();
}

template<typename T, typename T_c>
inline void ArrayTraits_Unordered_Toolkit<T, T_c>::destruct_D(T* data, int length)
{
  Q_ASSERT(data!=nullptr);
  Q_ASSERT(length>=0);

  for(int i=0; i<length; ++i)
    destruct_single_D(data+i);
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
  if(_data != nullptr)
  {
    traits::destruct(_data, _length);
    free_memory(_data);
  }
  _capacity = 0;
  _length = 0;
  _data = nullptr;

  traits::delete_cache(&this->trait_cache);
}

template<typename T, class T_traits>
Array<T, T_traits>::Array(const std::initializer_list<T>& init_with_values)
  : Array()
{
  ensureCapacity(init_with_values.size());
  for(const T& value : init_with_values)
    append_copy(value);
}

template<typename T, class T_traits>
Array<T, T_traits>::Array(Array&& other)
  : Array()
{
  this->swap(other);
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
  traits::swap_cache(&this->trait_cache, &other.trait_cache);
}


template<typename T, class T_traits>
int Array<T, T_traits>::capacity() const
{
  return _capacity;
}


template<typename T, class T_traits>
void Array<T, T_traits>::clear()
{
  if(_data != nullptr)
  {
    traits::destruct(_data, _length);
    free_memory(_data);
  }
  _capacity = 0;
  _length = 0;
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
      return;
    }

    T* old_data = this->_data;
    int old_length = this->_length;

    this->_data = allocate_memory(capacity);
    this->_capacity = capacity;

    if(this->_length > capacity)
    {
      this->_length = capacity;
      traits::capacity_reduced(capacity, &this->trait_cache);
    }

    // move the elements to the new buffer.
    traits::move_construct(this->_data, old_data, this->_length);

    if(old_data != nullptr)
    {
      traits::destruct(old_data, old_length);
      free_memory(old_data);
    }
  }
}

template<typename T, class T_traits>
void Array<T, T_traits>::ensureCapacity(int minCapacity)
{
  Q_ASSERT(capacity() >= 0);

  int oldCapacity = this->capacity();
  int newCapacity = T_traits::new_capacity(oldCapacity, this->length(), glm::max(0, minCapacity-this->length()));

  Q_ASSERT(newCapacity >= minCapacity);

  setCapacity(newCapacity);
}

template<typename T, class T_traits>
void Array<T, T_traits>::reserve(int minCapacity)
{
  Q_ASSERT(capacity() >= 0);

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
bool Array<T, T_traits>::isEmpty() const
{
  return _length==0;
}

template<typename T, class T_traits>
int Array<T, T_traits>::append_move(T&& value, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1));

  // the trait must assume, that there's enough space
  int new_index = traits::append_move(this->data(), this->length(), std::move(value), &this->trait_cache, hint);

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}

template<typename T, class T_traits>
int Array<T, T_traits>::extend_move(T* values, int num_values, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values));

  int new_index = traits::extend_move(this->data(), this->length(), values, num_values, &this->trait_cache, hint);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits>
int Array<T, T_traits>::append_copy(const T& value, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1));

  // the trait must assume, that there's enough space
  int new_index = traits::append_copy(this->data(), this->length(), value, &this->trait_cache, hint);

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}


template<typename T, class T_traits>
int Array<T, T_traits>::extend_copy(const T* values, int num_values, const hint_type& hint)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values));

  int new_index = traits::extend_copy(this->data(), this->length(), values, num_values, &this->trait_cache, hint);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits>
int Array<T, T_traits>::append(const T& value, const hint_type& hint)
{
  return append_copy(value, hint);
}

template<typename T, class T_traits>
int Array<T, T_traits>::append(T&& value, const hint_type& hint)
{
  return append_move(std::move(value), hint);
}

template<typename T, class T_traits>
void Array<T, T_traits>::remove(int index, const hint_type& hint)
{
  traits::remove_single(this->data(), this->length(), index, &this->trait_cache, hint);

  _length -= 1;
  Q_ASSERT(_length >= 0);

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), 1);
  setCapacity(new_capacity);
}

template<typename T, class T_traits>
void Array<T, T_traits>::remove(int index, int num_to_remove, const hint_type& hint)
{
  traits::remove(this->data(), this->length(), index, num_to_remove, &this->trait_cache, hint);

  _length -= num_to_remove;
  Q_ASSERT(_length >= 0);

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), num_to_remove);
  setCapacity(new_capacity);
}

template<typename T, class T_traits>
bool Array<T, T_traits>::operator==(const Array& other) const
{
  if(this->length() != other.length())
    return false;

  int n = this->length();

  for(int i=0; i<n; ++i)
  {
    if(this->at(i) != other.at(i))
      return false;
  }

  return true;
}

template<typename T, class T_traits>
bool Array<T, T_traits>::operator!=(const Array& other) const
{
  return !this->operator ==(other);
}

template<typename T, class T_traits>
QVector<T> Array<T, T_traits>::toQVector() const
{
  QVector<T> v;
  v.resize(this->length());
  for(int i=0; i<this->length(); ++i)
    v[i] = this->at(i);
  return v;
}


template<typename T, class T_traits>
T* Array<T, T_traits>::allocate_memory(int n)
{
  Q_ASSERT(n!=0);

  void* buffer = malloc(n*sizeof(T));

  if(buffer == nullptr)
    throw GLRT_EXCEPTION("Out of memory");

  return reinterpret_cast<T*>(buffer);
}

template<typename T, class T_traits>
void Array<T, T_traits>::free_memory(T* data)
{
  Q_ASSERT(data!=nullptr);
  free(data);
}


template<typename T, typename T_traits>
QDebug operator<<(QDebug d, const Array<T, T_traits>& array)
{
  return d << array.toQVector();
}


} // namespace glrt


template<typename T, class T_traits>
void std::swap(glrt::Array<T, T_traits>& a, glrt::Array<T, T_traits>& b)
{
  a.swap(b);
}


#endif // GLRT_ARRAY_INL
