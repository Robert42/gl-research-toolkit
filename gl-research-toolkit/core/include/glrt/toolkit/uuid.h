#ifndef GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL
#define GLRT_SCENE_RESOURCES_RESOURCEINDEX_INL

#include <glrt/dependencies.h>

#include <mutex>
#include <unordered_map>

namespace std {
  template<>
  struct hash<QUuid>
  {
  public:
    typedef QUuid argument_type;
    typedef std::size_t result_type;

    result_type operator()(const QUuid& uuid) const
    {
      // #TODO this is slow
      return hash<std::string>{}(uuid.toString().toStdString());
    }
  };
  template<>
  struct equal_to<QUuid>
  {
  public:
    size_t operator()(const QUuid& a, const QUuid& b) const
    {
      return a == b;
    }
  };
} // namespace std

namespace glrt {

class UuidIndex
{
public:
  typedef size_t index_type;

  index_type index;

  UuidIndex(const QUuid& uuid=QUuid());

  QUuid toQUuid() const;

private:
  static std::mutex& mutex();
  static std::unordered_map<QUuid, index_type>& allIndicesForUuid();
  static std::vector<QUuid>& allUuidsForIndex();

  static QUuid uuidForIndex(index_type i);
  static index_type indexForUuid(const QUuid& uuid);
};

template<class _identifier>
class Uuid final
{
public:
  typedef Uuid<_identifier> this_type;

  explicit Uuid(const QUuid& uuid=QUuid());
  explicit Uuid(UuidIndex index);

  template<typename T>
  Uuid(const Uuid<T>& _uuid) : _index(_uuid.index()) {static_assert(std::is_base_of<_identifier, T>::value, "T must inherit from: identifier");}
  Uuid(const Uuid<_identifier>& uuid) : _index(uuid.index()) {}

  template<typename T>
  Uuid<T> cast() const;

  operator QUuid() const;

  QUuid toQUuid() const;

  bool operator==(const this_type& other) const;
  bool operator!=(const this_type& other) const;
  bool operator>(const this_type& other) const;
  bool operator<(const this_type& other) const;
  bool operator>=(const this_type& other) const;
  bool operator<=(const this_type& other) const;

  static void registerAngelScriptAPI();
  static void registerCustomizedUuidType(const char* innerType, bool declareInnerType);

  UuidIndex index() const;

private:
  UuidIndex _index;
};

int qHash(UuidIndex uuidIndex);
template<class T>
int qHash(const Uuid<T>& uuid);


} // namespace scene


#include "uuid.inl"


#endif // GLRT_SCENE_RESOURCES_RESOURCEINDEX_H
