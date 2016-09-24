#ifndef GLRT_VARIABLEWITHCALLBACK_H
#define GLRT_VARIABLEWITHCALLBACK_H

#include <glrt/dependencies.h>

namespace glrt {

template<typename T>
class VariableWithCallback
{
public:
  QVector<std::function<void(T)>> callback_functions;
  std::function<T(T)> validate_value;

  VariableWithCallback(T initial_value);

  void set_value(T value);
  T get_value() const{return _value;}

  void connectWith(VariableWithCallback<T>* v);

  operator T() const{return get_value();}

private:
  T _value;
};

} // namespace glrt

#include "variable-with-callback.inl"

#endif // GLRT_VARIABLEWITHCALLBACK_H
