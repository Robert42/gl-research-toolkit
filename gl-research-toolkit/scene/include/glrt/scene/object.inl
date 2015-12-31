#ifndef GLRT_SCENE_OBJECT_INL
#define GLRT_SCENE_OBJECT_INL

#include "object.h"

namespace glrt {
namespace scene {

template<typename T>
void Object::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  static_assert(std::is_base_of<Object, T>::value, "T must inherit from glrt::Object");

  int r = 0;

  base_of<Object>::type::registerAsBaseOfClass<T>(engine, className);

  r = engine->RegisterObjectProperty(className, "const Uuid uuid", asOFFSET(T, uuid)); AngelScriptIntegration::AngelScriptCheck(r);
}

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_OBJECT_INL
