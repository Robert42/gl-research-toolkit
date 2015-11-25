#ifndef GLRT_SHADER_CACHE_H
#define GLRT_SHADER_CACHE_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace shader {

class Factory;
class Cache final
{
public:
  enum class Id : int
  {
    NONE = -1
  };

  Cache(const Cache&) = delete;
  Cache& operator=(const Cache&) = delete;
  Cache& operator=(const Cache&&) = delete;

  Cache();
  ~Cache();
  Cache(Cache&&);

  Id idForName(const QString& name);
  gl::ShaderObject& shaderForId(Id id);

  // Caller passes ownership to the given Factory instance
  void addShaderFactory(const QString& name, Factory* factory);

private:
  QHash<QString, Factory*> _shaderFactories;
  QHash<QString, Id> _idForName;
  QVector<gl::ShaderObject*> _allShaderObjects;
};


gl::ShaderObject& Cache::shaderForId(Id id)
{
  return *_allShaderObjects[int(id)];
}


} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_CACHE_H
