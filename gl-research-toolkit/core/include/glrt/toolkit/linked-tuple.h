#ifndef GLRT_LINKEDTUPLE_H
#define GLRT_LINKEDTUPLE_H

namespace glrt {

template<typename... T>
class LinkedTuple;

template<typename T>
class LinkedTuple<T> final
{
public:
  T head;

  LinkedTuple(const T& value)
    : head(value)
  {
  }
};

template<typename T, typename... T_tail>
class LinkedTuple<T, LinkedTuple<T_tail...>> final
{
public:
  T head;
  LinkedTuple<T_tail...> tail;

  LinkedTuple(const T& head_value, const T_tail&... tail_values)
    : head(head_value),
      tail(tail_values...)
  {
  }
};

template<typename T, typename... T_tail>
class LinkedTuple<T, T_tail...> final
{
public:
  T head;
  LinkedTuple<T_tail...> tail;

  LinkedTuple(const T& head_value, const T_tail&... tail_values)
    : head(head_value),
      tail(tail_values...)
  {
  }
};

template<typename... T>
LinkedTuple<T...> make_linked_tuple(const T&... values)
{
  return LinkedTuple<T...>(values...);
}

} // namespace glrt


#endif // GLRT_LINKEDTUPLE_H
