#ifndef GLRT_SCENE_OBJECT_H
#define GLRT_SCENE_OBJECT_H

#include "object.h"

namespace glrt {
namespace scene {

template<typename T>
void Object::registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className)
{
  base_of<Object>::registerAsBaseOfClass<T>(engine, className);
}

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_OBJECT_H
