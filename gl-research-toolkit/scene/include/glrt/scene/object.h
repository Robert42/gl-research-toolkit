#ifndef GLRT_SCENE_OBJECT_H
#define GLRT_SCENE_OBJECT_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {

class Object : public AngelScriptIntegration::RefCountedObject
{
public:
  const QUuid uuid;

protected:
  Object(const QUuid& uuid);

  template<typename T>
  static void registerAsBaseOfClass(AngelScript::asIScriptEngine* engine, const char* className);
};


} // namespace scene
} // namespace glrt

DECLARE_BASECLASS(AngelScriptIntegration::RefCountedObject, glrt::scene::Object);

#include "object.inl"

#endif // GLRT_SCENE_OBJECT_H
