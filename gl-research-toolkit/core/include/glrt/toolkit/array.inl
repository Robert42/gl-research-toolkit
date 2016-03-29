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

template<int block_size_append, int block_size_remove>
inline int ArrayCapacityTraits_Capacity_Blocks<block_size_append, block_size_remove>::recalc_capacity(int prev_capacity, int current_length)
{
  if(prev_capacity < current_length)
    return new_capacity(prev_capacity, current_length, current_length-prev_capacity);
  else if(prev_capacity > current_length)
    return adapt_capacity_after_removing_elements(prev_capacity, current_length, prev_capacity-current_length);
  else
    return prev_capacity;
}


/*! \class glrt::ArrayTraits_Toolkit

Trait for working with unordered arrays. When removing a value from the array,
it may be swapped with the last value, so the index of a value might change.

The good news: indicers are guaranteed to be the same, as long as no items are
removed. Even if so, two different array, both with ArrayTraits_Toolkit
traits will have the values consistently changed.

if removing one element, it is guaranted, that (if the removed element is not the
last one) the new index of the last value will be the previous index of the removed
value.
*/


template<typename T, typename T_c>
template<typename T_int>
inline bool ArrayTraits_Toolkit<T, T_c>::ranges_overlap(T_int range1_begin, T_int range1_end, T_int range2_begin, T_int range2_end)
{
  return (range1_begin>=range2_begin && range1_begin<range2_end)
      || (range1_end  > range2_begin && range1_end  <range2_end)
      || (range2_begin>=range1_begin && range2_begin<range1_end)
      || (range2_end  > range1_begin && range2_end  <range1_end);
}

template<typename T, typename T_c>
inline bool ArrayTraits_Toolkit<T, T_c>::ranges_overlap(const T* instanceA, const T* instanceB, int na, int nb)
{
  size_t a = size_t(instanceA);
  size_t b = size_t(instanceB);

  return ranges_overlap<size_t>(a, a+na*sizeof(T),
                                b, b+nb*sizeof(T));
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::swap_instances_mO(T* a, T* b, int n)
{
  Q_ASSERT(!ranges_overlap(a, b, n));

  for(int i=0; i<n; ++i)
    a[i] = std::move(b[i]);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::swap_single_instance_mO(T* a, T* b)
{
  Q_ASSERT(!ranges_overlap(a, b, 1));

  *a = std::move(*b);
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::copy_instances_aO(T* a, T* b, int n)
{
  Q_ASSERT(!ranges_overlap(a, b, n));

  for(int i=0; i<n; ++i)
    a[i] = b[i];
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::copy_single_instance_aO(T* a, T* b)
{
  Q_ASSERT(!ranges_overlap(a, b, 1));

  *a = *b;
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::call_instance_destructors_D(const T* a, int n)
{
  for(int i=0; i<n; ++i)
    (a+i)->~T();
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::values_used_to_fill_gaps(int* first, int* count, int prev_length, const int gap_start, int gap_length)
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
inline void ArrayTraits_Toolkit<T, T_c>::copy_construct_POD(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  std::memcpy(dest, src, sizeof(T)*count);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::copy_construct_single_POD(T* dest, const T* src)
{
  copy_construct_POD(dest, src, 1);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::copy_construct_single_cC(T* dest, const T* src)
{
  Q_ASSERT(!ranges_overlap(dest, src, 1));
  new(dest)T(*src);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::copy_construct_cC(T* dest, const T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  for(int i=0; i<count; ++i)
    copy_construct_single_cC(dest+i, src+i);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::move_construct_mC(T* dest, T* src, int count)
{
  Q_ASSERT(!ranges_overlap(dest, src, count));
  for(int i=0; i<count; ++i)
    move_construct_single_mC(dest+i, src+i);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::move_construct_single_mC(T* dest, T* src)
{
  Q_ASSERT(!ranges_overlap(dest, src, 1));
  new(dest)T(std::move(*src));
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::append_mC(T* data, int prev_length, T&& value)
{
  move_construct_single_mC(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::extend_mC(T* data, int prev_length, T* values, int num_values)
{
  move_construct_mC(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::append_Primitive(T* data, int prev_length, T value)
{
  data[prev_length] = value;
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::append_POD(T* data, int prev_length, const T& value)
{
  copy_construct_single_POD(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::extend_POD(T* data, int prev_length, const T* values, int num_values)
{
  copy_construct_POD(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::append_cC(T* data, int prev_length, const T& value)
{
  copy_construct_single_cC(data+prev_length, &value);
  return prev_length;
}

template<typename T, typename T_c>
inline int ArrayTraits_Toolkit<T, T_c>::extend_cC(T* data, int prev_length, const T* values, int num_values)
{
  copy_construct_cC(data+prev_length, values, num_values);
  return prev_length;
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::remove_single_POD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    copy_construct_single_POD(data+index, data+last);
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::remove_single_Primitive(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  data[index] = data[last];
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::remove_POD(T* data, int prev_length, const int first_index, int num_values)
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
inline void ArrayTraits_Toolkit<T, T_c>::remove_single_mOD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    swap_single_instance_mO(data+index, data + last);
  destruct_single_D(data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::remove_mOD(T* data, int prev_length, const int first_index, int num_values)
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
inline void ArrayTraits_Toolkit<T, T_c>::remove_single_aOD(T* data, int prev_length, const int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<prev_length);

  int last = prev_length-1;
  if(index != last)
    copy_single_instance_aO(data+index, data + last);
  destruct_single_D(data+last);
}

template<typename T, typename T_c>
void ArrayTraits_Toolkit<T, T_c>::remove_aOD(T* data, int prev_length, const int first_index, int num_values)
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
inline void ArrayTraits_Toolkit<T, T_c>::remove_single_cCD(T* data, int prev_length, const int index)
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
void ArrayTraits_Toolkit<T, T_c>::remove_cCD(T* data, int prev_length, const int first_index, int num_values)
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
inline void ArrayTraits_Toolkit<T, T_c>::destruct_single_D(T* data)
{
  Q_ASSERT(data!=nullptr);

  (data)->~T();
}

template<typename T, typename T_c>
inline void ArrayTraits_Toolkit<T, T_c>::destruct_D(T* data, int length)
{
  Q_ASSERT(data!=nullptr);
  Q_ASSERT(length>=0);

  for(int i=0; i<length; ++i)
    destruct_single_D(data+i);
}

// =============================================================================


template<typename T>
T* DefaultAllocator<T>::allocate_memory(int n)
{
  Q_ASSERT(n!=0);

  void* buffer = malloc(n*sizeof(T));

  if(buffer == nullptr)
    throw GLRT_EXCEPTION("Out of memory");

  return reinterpret_cast<T*>(buffer);
}

template<typename T>
void DefaultAllocator<T>::free_memory(T* data)
{
  Q_ASSERT(data!=nullptr);
  free(data);
}

// -----------------------------------------------------------------------------

template<typename T, class T_prepended_type, int offset>
T* AllocatorWithPrependedData<T,T_prepended_type,offset>::allocate_memory(int n)
{
  Q_ASSERT(n!=0);

  quint8* whole_buffer = reinterpret_cast<quint8*>(malloc(n*sizeof(T) + offset));

  if(whole_buffer == nullptr)
    throw GLRT_EXCEPTION("Out of memory");

  return data_buffer<T>(whole_buffer);
}

template<typename T, class T_prepended_type, int offset>
void AllocatorWithPrependedData<T,T_prepended_type,offset>::free_memory(T* data)
{
  Q_ASSERT(data!=nullptr);
  free(whole_buffer<quint8>(data));
}

template<typename T, class T_prepended_type, int offset>
T_prepended_type& AllocatorWithPrependedData<T,T_prepended_type,offset>::prepended_data(T* data)
{
  return *whole_buffer<T_prepended_type>(data);
}

template<typename T, class T_prepended_type, int offset>
const T_prepended_type& AllocatorWithPrependedData<T,T_prepended_type,offset>::prepended_data(const T* data)
{
  return *whole_buffer<const T_prepended_type>(data);
}

template<typename T, class T_prepended_type, int offset>
template<typename T_whole_buffer, typename T_data_buffer>
T_whole_buffer* AllocatorWithPrependedData<T,T_prepended_type,offset>::whole_buffer(T_data_buffer* data_buffer)
{
  quint8* whole_buffer = ((quint8*)data_buffer) - offset;

  return ((T_whole_buffer*)whole_buffer);
}

template<typename T, class T_prepended_type, int offset>
template<typename T_data_buffer, typename T_whole_buffer>
T_data_buffer* AllocatorWithPrependedData<T,T_prepended_type,offset>::data_buffer(T_whole_buffer* whole_buffer)
{
  quint8* data_buffer = ((quint8*)whole_buffer) + offset;

  return ((T_data_buffer*)data_buffer);
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


template<typename T, class T_traits, class T_allocator>
Array<T, T_traits, T_allocator>::Array()
  : _data(nullptr),
    _capacity(0),
    _length(0)
{
}

template<typename T, class T_traits, class T_allocator>
Array<T, T_traits,T_allocator>::~Array()
{
  if(_data != nullptr)
  {
    traits::destruct(_data, _length);
    allocator::free_memory(_data);
  }
  _capacity = 0;
  _length = 0;
  _data = nullptr;
}

template<typename T, class T_traits, class T_allocator>
Array<T, T_traits,T_allocator>::Array(const std::initializer_list<T>& init_with_values)
  : Array()
{
  ensureCapacity(init_with_values.size());
  for(const T& value : init_with_values)
    append_copy(value);
}

template<typename T, class T_traits, class T_allocator>
Array<T, T_traits,T_allocator>::Array(Array&& other)
  : Array()
{
  this->swap(other);
}

template<typename T, class T_traits, class T_allocator>
Array<T, T_traits,T_allocator>& Array<T, T_traits,T_allocator>::operator=(Array<T, T_traits,T_allocator>&& other)
{
  this->swap(other);
  return *this;
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::swap(Array& other)
{
  std::swap(this->_data, other._data);
  std::swap(this->_capacity, other._capacity);
  std::swap(this->_length, other._length);
}


template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::capacity() const
{
  return _capacity;
}


template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::clear()
{
  if(_data != nullptr)
  {
    traits::destruct(_data, _length);
    allocator::free_memory(_data);
  }
  _capacity = 0;
  _length = 0;
  _data = nullptr;
}

template<typename T, class T_traits, class T_allocator>
T* Array<T, T_traits,T_allocator>::begin()
{
  return this->_data;
}

template<typename T, class T_traits, class T_allocator>
T* Array<T, T_traits,T_allocator>::end()
{
  return this->_data + this->_length;
}

template<typename T, class T_traits, class T_allocator>
const T* Array<T, T_traits,T_allocator>::begin() const
{
  return this->constBegin();
}

template<typename T, class T_traits, class T_allocator>
const T* Array<T, T_traits,T_allocator>::end() const
{
  return this->constEnd();
}

template<typename T, class T_traits, class T_allocator>
const T* Array<T, T_traits,T_allocator>::constBegin() const
{
  return this->_data;
}

template<typename T, class T_traits, class T_allocator>
const T* Array<T, T_traits,T_allocator>::constEnd() const
{
  return this->_data + this->_length;
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::setCapacity(int capacity)
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

    this->_data = allocator::allocate_memory(capacity);
    this->_capacity = capacity;

    if(this->_length > capacity)
    {
      this->_length = capacity;
    }

    // move the elements to the new buffer.
    traits::move_construct(this->_data, old_data, this->_length);

    if(old_data != nullptr)
    {
      traits::destruct(old_data, old_length);
      allocator::free_memory(old_data);
    }
  }
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::ensureCapacity(int minCapacity)
{
  Q_ASSERT(capacity() >= 0);

  int oldCapacity = this->capacity();
  int newCapacity = T_traits::new_capacity(oldCapacity, this->length(), glm::max(0, minCapacity-this->length()));

  Q_ASSERT(newCapacity >= minCapacity);

  setCapacity(newCapacity);
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::reserve(int minCapacity)
{
  Q_ASSERT(capacity() >= 0);

  ensureCapacity(minCapacity);
}


template<typename T, class T_traits, class T_allocator>
T* Array<T, T_traits,T_allocator>::data()
{
  return _data;
}

template<typename T, class T_traits, class T_allocator>
const T* Array<T, T_traits,T_allocator>::data() const
{
  return _data;
}


template<typename T, class T_traits, class T_allocator>
T& Array<T, T_traits,T_allocator>::at(int i)
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}

template<typename T, class T_traits, class T_allocator>
const T& Array<T, T_traits,T_allocator>::at(int i) const
{
  Q_ASSERT(i>=0);
  Q_ASSERT(i<_length);
  return *(_data+i);
}


template<typename T, class T_traits, class T_allocator>
T& Array<T, T_traits,T_allocator>::operator[](int i)
{
  return at(i);
}

template<typename T, class T_traits, class T_allocator>
const T& Array<T, T_traits,T_allocator>::operator[](int i) const
{
  return at(i);
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::length() const
{
  return _length;
}

template<typename T, class T_traits, class T_allocator>
bool Array<T, T_traits,T_allocator>::isEmpty() const
{
  return _length==0;
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::append_move(T&& value)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1));

  // the trait must assume, that there's enough space
  int new_index = traits::append_move(this->data(), this->length(), std::move(value));

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::extend_move(T* values, int num_values)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values));

  int new_index = traits::extend_move(this->data(), this->length(), values, num_values);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::append_copy(const T& value)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), 1));

  // the trait must assume, that there's enough space
  int new_index = traits::append_copy(this->data(), this->length(), value);

  _length++;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<_length);

  return new_index;
}


template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::extend_copy(const T* values, int num_values)
{
  ensureCapacity(traits::new_capacity(this->capacity(), this->length(), num_values));

  int new_index = traits::extend_copy(this->data(), this->length(), values, num_values);

  _length += num_values;

  Q_ASSERT(new_index>=0);
  Q_ASSERT(new_index<=_length-num_values);

  return new_index;
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::append(const T& value)
{
  return append_copy(value);
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::append(T&& value)
{
  return append_move(std::move(value));
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::removeAt(int index)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index<this->length());

  traits::remove_single(this->data(), this->length(), index);

  _length -= 1;
  Q_ASSERT(_length >= 0);

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), 1);
  setCapacity(new_capacity);
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::removeAt(int index, int num_to_remove)
{
  Q_ASSERT(index>=0);
  Q_ASSERT(index+num_to_remove<=this->length());

  traits::remove(this->data(), this->length(), index, num_to_remove);

  _length -= num_to_remove;
  Q_ASSERT(_length >= 0);

  int new_capacity = traits::adapt_capacity_after_removing_elements(this->capacity(), this->length(), num_to_remove);
  setCapacity(new_capacity);
}

template<typename T, class T_traits, class T_allocator>
int Array<T, T_traits,T_allocator>::indexOfFirst(const T& value, int fallback) const
{
  const T* data = this->data();
  const int length = this->length();
  for(int i=0; i<length; ++i)
    if(data[i] == value)
      return i;
  return fallback;
}

template<typename T, class T_traits, class T_allocator>
bool Array<T, T_traits,T_allocator>::operator==(const Array& other) const
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

template<typename T, class T_traits, class T_allocator>
bool Array<T, T_traits,T_allocator>::operator!=(const Array& other) const
{
  return !this->operator ==(other);
}

template<typename T, class T_traits, class T_allocator>
QVector<T> Array<T, T_traits,T_allocator>::toQVector() const
{
  QVector<T> v;
  v.resize(this->length());
  for(int i=0; i<this->length(); ++i)
    v[i] = this->at(i);
  return v;
}

template<typename T, class T_traits, class T_allocator>
template<typename T_lessThan>
void Array<T, T_traits,T_allocator>::stable_sort(T_lessThan lessThan)
{
  // #ISSUE-61 STL
  std::stable_sort(this->data(), this->data()+this->length(), lessThan);
}

template<typename T, class T_traits, class T_allocator>
template<typename T_lessThan>
void Array<T, T_traits,T_allocator>::sort(T_lessThan lessThan)
{
  // #ISSUE-61 STL
  std::sort(this->data(), this->data()+this->length(), lessThan);
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::append_by_memcpy(const void* src, size_t num)
{
  static_assert(std::is_same<byte, T>::value, "append_by_memcpy only allowed for an array of the type byte");

  Q_ASSERT_X(src<this->_data || src >= this->_data+this->_capacity, "Array<>::append_by_memcpy()", "src must be from a buffer outside the buffer of the current array. Otherwise, reserve might invalidate the buffer");

  reserve(length() + num);
  std::memcpy(this->_data + this->_length, src, num);
  this->_length += num;
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::append_by_memcpy_items(int first, int num)
{
  static_assert(std::is_same<byte, T>::value, "append_by_memcpy only allowed for an array of the type byte");

  Q_ASSERT(first>=0);
  Q_ASSERT(num>=0);
  Q_ASSERT(first+num<=this->length());

  reserve(length() + num);
  std::memcpy(this->_data + this->_length, this->_data+first, num);
  this->_length += num;
}

template<typename T, class T_traits, class T_allocator>
template<typename T_value>
void Array<T, T_traits,T_allocator>::append_by_memcpy(const T_value& value)
{
  append_by_memcpy(&value, sizeof(value));
}

template<typename T, class T_traits, class T_allocator>
void Array<T, T_traits,T_allocator>::resize(int newSize)
{
  static_assert(std::is_same<byte, T>::value, "append_by_memcpy only allowed for an array of the type byte");

  Q_ASSERT(newSize>=0);


  if(newSize <= 0)
    clear();
  else
  {
    ensureCapacity(newSize);
    _length = newSize;
  }
}


template<typename T, typename T_traits, class T_allocator>
QDebug operator<<(QDebug d, const Array<T, T_traits,T_allocator>& array)
{
  return d << array.toQVector();
}


} // namespace glrt


template<typename T, class T_traits, class T_allocator>
void std::swap(glrt::Array<T, T_traits,T_allocator>& a, glrt::Array<T, T_traits,T_allocator>& b)
{
  a.swap(b);
}


#endif // GLRT_ARRAY_INL
