#include <glrt/toolkit/array.h>

#include <testing-framework.h>

using glrt::Array;

template<typename T>
struct TestTrait
{
  typedef QString hint_type;
  typedef QString cache_type;

  static const QString default_append_hint(){return QString("default_append_hint");}
  static const QString default_remove_hint(){return "default_remove_hint";}


  static void change_location(T* dest, const T* src, int count)
  {
    summary() += QString("change_location(dest: %0, src: %1, count: %2)\n").arg(index_from_ptr(dest)).arg(index_from_ptr(src)).arg(count);
  }

  static void change_location_single(T* dest, const T* src)
  {
    summary() += QString("change_location(dest: %0, src: %1)\n").arg(index_from_ptr(dest));
  }

  static int new_capacity(int prev_capacity, int current_length, int elements_to_add, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache());

    summary() += QString("new_capacity(prev_capacity: %0, current_length: %1, elements_to_add: %2)\n").arg(prev_capacity).arg(current_length).arg(elements_to_add);

    return glm::max(prev_capacity, current_length+elements_to_add);
  }

  static int adapt_capacity_after_removing_elements(int prev_capacity, int current_length, int elements_removed, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache());

    summary() += QString("adapt_capacity_after_removing_elements(prev_capacity: %0, current_length: %1, elements_removed: %2)\n").arg(prev_capacity).arg(current_length).arg(elements_removed);

    return glm::max(prev_capacity, current_length);
  }

  static int index_from_ptr(const T* t)
  {
    ASSERT_TRUE(size_t(t)>=size_t(data_start()));

    return int(size_t(t)-size_t(data_start()));
  }

  static void init_cache(cache_type* c)
  {
    TestTrait<T>::cache_ptr() = c;

    *c = "init_cache";
  }

  static void clear_cache(cache_type* c)
  {
    EXPECT_EQ(c, TestTrait<T>::cache_ptr());

    *c = "clear_cache";
  }

  static void delete_cache(cache_type* c)
  {
    EXPECT_EQ(c, TestTrait<T>::cache_ptr());

    static QString s;

    TestTrait<T>::cache_ptr() = &s;
    *TestTrait<T>::cache_ptr() = "delete_cache";
  }

  static void swap_cache(cache_type* a, cache_type* b)
  {
    EXPECT_EQ(a, TestTrait<T>::cache_ptr());
    EXPECT_EQ(b, TestTrait<T>::cache_ptr());

    *a = "swap_cache a";
    *b = "swap_cache b";
  }

  static int append_move(T* data, int prev_length, T&& value, const hint_type& hint, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache_ptr());

    summary() += QString("append_move(data: %0, prev_length: %1, value: %2, hint: %3)\n").arg(index_from_ptr(data)).arg(prev_length).arg(value).arg(hint);
    return prev_length;
  }

  static int append(T* data, int prev_length, const T& value, const hint_type& hint, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache_ptr());

    summary() += QString("append(data: %0, prev_length: %1, value: %2, hint: %3)\n").arg(index_from_ptr(data)).arg(prev_length).arg(value).arg(hint);
    return 0;
  }

  static int extend(T* data, int prev_length, const T*, int num_values, const hint_type& hint, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache_ptr());

    summary() += QString("extend(data: %0, prev_length: %1, values: %2, num_values: %3, hint: %4)\n").arg(index_from_ptr(data)).arg(prev_length).arg(num_values).arg(hint);
    return 0;
  }

  static void remove_single(T* data, int prev_length, const int index, const hint_type& hint, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache_ptr());

    summary() += QString("remove_single(data: %0, prev_length: %1, index: %2, hint: %4)\n").arg(index_from_ptr(data)).arg(prev_length).arg(index).arg(hint);
    return 0;
  }

  static void remove(T* data, int prev_length, const int first_index, int num_values, const hint_type& hint, cache_type* cache)
  {
    EXPECT_EQ(cache, TestTrait<T>::cache_ptr());

    summary() += QString("remove(data: %0, prev_length: %1, values: %2, index: %3, hint: %4)\n").arg(index_from_ptr(data)).arg(prev_length).arg(first_index).arg(num_values).arg(hint);
    return 0;
  }


  static QString& summary()
  {
    static QString str;
    return str;
  }

  static T*& data_start()
  {
    static T* data = nullptr;
    return data;
  }

  static cache_type*& cache_ptr()
  {
    static cache_type* data = nullptr;
    return data;
  }

};

void test_trait_handling()
{
  {
    Array<int, TestTrait<int>> array;
    EXPECT_EQ(*TestTrait<int>::cache_ptr(), "init_cache");
  }
  EXPECT_EQ(*TestTrait<int>::cache_ptr(), "delete_cache");
}
