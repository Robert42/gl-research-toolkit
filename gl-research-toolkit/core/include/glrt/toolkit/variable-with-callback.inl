#ifndef GLRT_VARIABLEWITHCALLBACK_INL
#define GLRT_VARIABLEWITHCALLBACK_INL

#include <glrt/toolkit/variable-with-callback.h>

namespace glrt {


template<typename T>
VariableWithCallback<T>::VariableWithCallback(T initial_value)
  : VariableWithCallback(initial_value,
                         [](T v){return v;})
{
}

template<typename T>
VariableWithCallback<T>::VariableWithCallback(T initial_value, const std::function<T(T)>& validate_value)
  : validate_value(validate_value),
    _value(initial_value)
{
}

template<typename T>
void VariableWithCallback<T>::set_value(T value)
{
  value = validate_value(value);

  if(value == _value)
    return;

  _value = value;

  for(const std::function<void(T)>& fn : callback_functions)
    fn(value);
}

template<typename T>
inline void VariableWithCallback<T>::connectWith(VariableWithCallback<T>* v)
{
  set_value(v->get_value());
  v->callback_functions.push_back([this](T v){this->set_value(v);});
}


} // namespace glrt

#endif // GLRT_VARIABLEWITHCALLBACK_INL
