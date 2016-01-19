#ifndef GLRT_ARRAYBUCKETTRAITS_H
#define GLRT_ARRAYBUCKETTRAITS_H

#include <glrt/toolkit/array.h>

namespace glrt {
namespace implementation {


template<typename T_this_hint, typename T_inner_hint>
struct combined_hint_type
{
  typedef LinkedTuple<T_this_hint, T_inner_hint> type;

  static T_this_hint current_hint(const type& hint)
  {
    return hint.head;
  }

  static const T_inner_hint& inner_hint(const type& hint)
  {
    return hint.tail.head;
  }

  static type make_default_hint(const T_this_hint& value, const T_inner_hint& inner_value)
  {
    return make_linked_tuple<T_this_hint, T_inner_hint>(value, inner_value);
  }
};

template<typename T_this_hint, typename... T_inner_hint>
struct combined_hint_type<T_this_hint, LinkedTuple<T_inner_hint...>>
{
  typedef LinkedTuple<T_this_hint, T_inner_hint...> type;

  static T_this_hint current_hint(const type& hint)
  {
    return hint.head;
  }

  static const LinkedTuple<T_inner_hint...>& inner_hint(const type& hint)
  {
    return hint.tail;
  }

  static type make_default_hint(const T_this_hint& value, const LinkedTuple<T_inner_hint...>& inner_value)
  {
    return make_linked_tuple<T_this_hint, T_inner_hint...>(value, inner_value);
  }
};

template<typename T_this_hint>
struct combined_hint_type<T_this_hint, dummy_array_hint_type>
{
  typedef T_this_hint type;

  static T_this_hint current_hint(const type& hint)
  {
    return hint;
  }

  static dummy_array_hint_type inner_hint(const type&)
  {
    return dummy_array_hint_type::DefaultHint;
  }

  static type make_default_hint(const T_this_hint& value, dummy_array_hint_type)
  {
    return value;
  }
};


template<typename T_this_cache, typename T_inner_cache>
struct combined_cache_type
{
  typedef LinkedTuple<T_this_cache, T_inner_cache> type;

  static T_this_cache* current_cache(type* cache)
  {
    return &cache->head;
  }

  static T_inner_cache* inner_cache(type* cache)
  {
    return &cache->tail.head;
  }
};

template<typename T_this_cache, typename... T_inner_cache>
struct combined_cache_type<T_this_cache, LinkedTuple<T_inner_cache...>>
{
  typedef LinkedTuple<T_this_cache, T_inner_cache...> type;

  static T_this_cache* current_cache(type* cache)
  {
    return &cache->head;
  }

  static LinkedTuple<T_inner_cache...>* inner_cache(type* cache)
  {
    return &cache->tail;
  }
};

template<typename T_this_cache>
struct combined_cache_type<T_this_cache, dummy_array_cache_type>
{
  typedef T_this_cache type;

  static T_this_cache* current_cache(type* cache)
  {
    return cache;
  }

  static dummy_array_cache_type* inner_cache(type*)
  {
    return nullptr;
  }
};


struct ArrayBucketTraits_BucketCache_VariableNum
{
  typedef Array<int> cache_type;

  static void init_cache(cache_type*)
  {
  }

  static void clear_cache(cache_type* cache)
  {
    cache->clear();
  }

  static void delete_cache(cache_type*)
  {
  }

  static void swap_cache(cache_type* cache1, cache_type* cache2)
  {
    cache1->swap(*cache2);
  }

  static void capacity_reduced(int capacity, cache_type* cache)
  {
    // #TODO
  }

  static int* bucketLimits(cache_type* cache)
  {
    return cache->data();
  }

  static int numberBucketsAvialable(cache_type* cache)
  {
    return cache->length();
  }

  static void add_buckets_if_necessary(int requestedNumBuckets, cache_type* cache)
  {
    while(requestedNumBuckets > cache->length())
      cache->append(0);
  }
};


template<int N>
struct ArrayBucketTraits_BucketCache_FixedNum
{
  typedef int cache_type[N];

  static void init_cache(cache_type* cache)
  {
    int* v = bucketLimits(cache);
    for(int i=0; i<N; ++i)
      v[i] = 0;
  }

  static void clear_cache(cache_type* cache)
  {
    int* v = bucketLimits(cache);
    for(int i=0; i<N; ++i)
      v[i] = 0;
  }

  static void delete_cache(cache_type*)
  {
  }

  static void swap_cache(cache_type* cache1, cache_type* cache2)
  {
    int* va = bucketLimits(cache1);
    int* vb = bucketLimits(cache2);
    for(int i=0; i<N; ++i)
      std::swap(va[i], vb[i]);
  }

  static void capacity_reduced(int, cache_type*)
  {
  }


  static int* bucketLimits(cache_type* cache)
  {
    return *cache;
  }

  static int numberBucketsAvialable(cache_type*)
  {
    return N;
  }

  static void add_buckets_if_necessary(int requestedNumBuckets, cache_type*)
  {
    qCritical() << "ArrayBucketTraits_BucketCache_FixedNum<"<<N<<">: requestedNumBuckets ("<<requestedNumBuckets<<") is not allowed to be larger than N ("<<N<<")";
    Q_ASSERT(requestedNumBuckets < N);
  }
};


template<typename T, typename T_cache_traits, typename T_inner_traits>
struct ArrayBucketTraits_ByNumberOfBuckets_Base : public T_inner_traits
{
  typedef T_inner_traits inner_traits;

  typedef T_cache_traits cache_traits;

  typedef combined_hint_type<int, typename inner_traits::hint_type> _hint_helper;
  typedef combined_cache_type<typename cache_traits::cache_type, typename inner_traits::cache_type> _cache_helper;

  typedef typename _hint_helper::type hint_type;
  typedef typename _cache_helper::type cache_type;


  static hint_type default_append_hint(){return make_default_hint(0, inner_traits::default_append_hint());}
  static hint_type default_remove_hint(){return make_default_hint(0, inner_traits::default_remove_hint());}

  static void init_cache(cache_type* cache)
  {
    cache_traits::init_cache(_cache_helper::current_chache(cache));
    inner_traits::init_cache(_cache_helper::inner_cache(cache));
  }

  static void clear_cache(cache_type* cache)
  {
    cache_traits::clear_cache(_cache_helper::current_chache(cache));
    inner_traits::clear_cache(_cache_helper::inner_cache(cache));
  }

  static void delete_cache(cache_type* cache)
  {
    cache_traits::delete_cache(_cache_helper::current_chache(cache));
    inner_traits::delete_cache(_cache_helper::inner_cache(cache));
  }

  static void swap_cache(cache_type* cache1, cache_type* cache2)
  {
    cache_traits::swap_cache(_cache_helper::current_chache(cache1->head),
                             _cache_helper::current_chache(cache2->head));
    inner_traits::swap_cache(_cache_helper::inner_cache(cache1),
                             _cache_helper::inner_cache(cache2));
  }

  static void capacity_reduced(int capacity, cache_type* cache)
  {
    cache_traits::capacity_reduced(capacity, _cache_helper::current_chache(cache));
    inner_traits::capacity_reduced(capacity, _cache_helper::inner_cache(cache));
  }


  static int append_move(T* data, int prev_length, T&& value, cache_type* cache, const hint_type& hint)
  {
    int index_offset = prepare_buckets_for_adding_values(data, prev_length, 1, cache, hint);

    return index_offset + inner_traits::append_move(data, prev_length, std::move(value), _cache_helper::inner_cache(cache), _hint_helper::inner_hint(hint));
  }

  static int extend_move(T* data, int prev_length, const T* values, int num_values, cache_type* cache, const hint_type& hint)
  {
    int index_offset = prepare_buckets_for_adding_values(data, prev_length, num_values, cache, hint);

    return index_offset + inner_traits::extend_move(data, prev_length, values, _cache_helper::inner_cache(cache), _hint_helper::inner_hint(hint));
  }

  static int append_copy(T* data, int prev_length, const T& value, cache_type* cache, const hint_type& hint)
  {
    int index_offset = prepare_buckets_for_adding_values(data, prev_length, 1, cache, hint);

    return index_offset + inner_traits::append_copy(data, prev_length, value, _cache_helper::inner_cache(cache), _hint_helper::inner_hint(hint));
  }

  static int extend_copy(T* data, int prev_length, const T* values, int num_values, cache_type* cache, const hint_type& hint)
  {
    int index_offset = prepare_buckets_for_adding_values(data, prev_length, num_values, cache, hint);

    return index_offset + inner_traits::extend_copy(data, prev_length, values, _cache_helper::inner_cache(cache), _hint_helper::inner_hint(hint));
  }

  static void remove_single(T* data, int prev_length, const int index, cache_type* cache, const hint_type& hint);  // #TODO implement

  static void remove(T* data, int prev_length, const int first_index, int num_values, cache_type* cache, const hint_type& hint); // #TODO implement

protected:
  static int get_bucketId(const hint_type& hint)
  {
    int bucketId = _hint_helper::current_hint(hint);
    Q_ASSERT(bucketId >= 0);
    return bucketId;
  }

  static int* get_bucketLimits(cache_type* cache)
  {
    int* bucketLimits = cache_traits::bucketLimits(_hint_helper::current_cache(cache));

    Q_ASSERT(bucketLimits!=nullptr);

    return bucketLimits;
  }

  static int get_numberBucketsAvialable(cache_type* cache)
  {
    int nBucketLimits = cache_traits::numberBucketsAvialable(_hint_helper::current_cache(cache));
    Q_ASSERT(nBucketLimits >= 0);
    return nBucketLimits;
  }

  static int prepare_buckets_for_adding_values(T*& data, int& length, int nBucketsToAdd, cache_type* cache, const hint_type& hint)
  {
    int bucketId = get_bucketId(hint);
    int* bucketLimits = get_bucketLimits(cache);
    int nBucketLimits = get_numberBucketsAvialable(cache);

    cache_traits::add_buckets_if_necessary(bucketId, _hint_helper::current_cache(cache));

    int bucketBegin = get_bucket_begin(bucketId, bucketLimits, nBucketLimits, length);
    int bucketEnd = get_bucket_end(bucketId, bucketLimits, nBucketLimits, length);

    inner_traits::insert_uninitialized_gap_by_expanding_to_uninitialized_area(data, length, bucketEnd, nBucketsToAdd); // #TODO implement

    adaptLimits(bucketId, bucketLimits, nBucketLimits, nBucketsToAdd);

    data += bucketBegin;
    length = bucketEnd - bucketBegin;

    return bucketBegin;
  }

  static int get_bucket_begin(int bucketId, const int* bucketLimits, int nBucketLimits, int bufferLength)
  {
    Q_ASSERT(nBucketLimits > bucketId);

    int bucketBegin = bucketId>0 ? bucketLimits[bucketId-1] : 0;
    int bucketEnd = bucketLimits[bucketId];

    Q_ASSERT(bucketBegin >= 0);
    Q_ASSERT(bucketEnd >= 0);
    Q_ASSERT(bucketBegin <= bucketEnd);
    Q_ASSERT(bufferLength >= bucketEnd);

    return bucketBegin;
  }

  static int get_bucket_end(int bucketId, const int* bucketLimits, int nBucketLimits, int bufferLength)
  {
    Q_ASSERT(nBucketLimits > bucketId);

    int bucketEnd = bucketLimits[bucketId];

    Q_ASSERT(bucketEnd >= 0);
    Q_ASSERT(bufferLength >= bucketEnd);

    return bucketEnd;
  }

  static void adaptLimits(int bucketId, int* bucketLimits, int nBucketLimits, int offset)
  {
    for(int i=bucketId; i<nBucketLimits; ++i)
      bucketLimits[i] += offset;
  }
};


} // namespace implementation


template<typename T, typename T_inner_traits>
struct ArrayBucketTraits_VariableNumberOfBuckets : public implementation::ArrayBucketTraits_ByNumberOfBuckets_Base<T, implementation::ArrayBucketTraits_BucketCache_VariableNum, T_inner_traits>
{
};

template<typename T, typename T_inner_traits, int N>
struct ArrayBucketTraits_FixedNumberOfBuckets : public implementation::ArrayBucketTraits_ByNumberOfBuckets_Base<T, implementation::ArrayBucketTraits_BucketCache_FixedNum<N>, T_inner_traits>
{
};


} // namespace glrt

#endif // GLRT_ARRAYBUCKETTRAITS_H
