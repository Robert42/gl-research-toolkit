#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL

#include <glrt/dependencies.h>

namespace glrt {


template<class _identifier>
class Uuid final
{
public:
  explicit Uuid(const QUuid& uuid=QUuid());

  template<typename T>
  Uuid(const Uuid<T>& uuid) : _uuid(uuid) {static_assert(std::is_base_of<_identifier, T>::value, "T must inherit from :identifier");}
  Uuid(const Uuid<_identifier>& uuid) : _uuid(uuid) {}

  operator const QUuid&() const;
  bool operator==(const QUuid& other) const;
  bool operator!=(const QUuid& other) const;
  bool operator>(const QUuid& other) const;
  bool operator<(const QUuid& other) const;
  bool operator>=(const QUuid& other) const;
  bool operator<=(const QUuid& other) const;

  static void registerAngelScriptAPI();
  static void registerCustomizedUuidType(const char* innerType, bool declareInnerType);

private:
  QUuid _uuid;
};

template<class T>
int qHash(const Uuid<T>& uuid, uint seed=0);


} // namespace scene


#include "uuid.inl"


#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
