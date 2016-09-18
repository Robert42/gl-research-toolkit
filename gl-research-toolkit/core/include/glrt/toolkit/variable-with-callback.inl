#ifndef GLRT_VARIABLEWITHCALLBACK_INL
#define GLRT_VARIABLEWITHCALLBACK_INL

#include <glrt/toolkit/variable-with-callback.h>

namespace glrt {


template<typename T>
VariableWithCallback<T>::VariableWithCallback(T initial_value)
  : _value(initial_value)
{
  validate_value = [](T v){return v;};
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


} // namespace glrt

#endif // GLRT_VARIABLEWITHCALLBACK_INL
