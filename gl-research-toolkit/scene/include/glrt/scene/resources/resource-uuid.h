#ifndef GLRT_SCENE_RESOURCES_UUID_H
#define GLRT_SCENE_RESOURCES_UUID_H

#include <glrt/dependencies.h>

namespace glrt {
namespace scene {
namespace resources {

enum class State
{
  NONE,
  REGISTERED,
  LOADING,
  LOADED
};

class ResourceIndex;
class ResourceLoader;

template<class _identifier>
class Uuid
{
public:
  explicit Uuid(const QUuid& uuid);

  operator const QUuid&() const;
  bool operator==(const QUuid& other) const;
  bool operator!=(const QUuid& other) const;
  bool operator>(const QUuid& other) const;
  bool operator<(const QUuid& other) const;
  bool operator>=(const QUuid& other) const;
  bool operator<=(const QUuid& other) const;

  static void registerAngelScriptAPI();

private:
  QUuid _uuid;
};

template<class T>
int qHash(const Uuid<T>& uuid, uint seed=0);


class StaticMeshData;
class MaterialData;
typedef Uuid<StaticMeshData> StaticMeshUuid;
typedef Uuid<MaterialData> MaterialDataUuid;


void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, QUuid* value);
void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, StaticMeshUuid* value);
void pass_arg_to_angelscript(AngelScript::asIScriptContext*context, int i, MaterialDataUuid* value);


} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-uuid.inl"

#endif // GLRT_SCENE_RESOURCES_UUID_H
