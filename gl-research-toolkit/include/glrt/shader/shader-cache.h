#ifndef GLRT_SHADER_SHADERCACHE_H
#define GLRT_SHADER_SHADERCACHE_H

#include <glrt/dependencies.h>

#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace shader {

class ShaderFactory;
class ShaderCache final
{
public:
  enum class Id : int
  {
    NONE = -1
  };

  ShaderCache(const ShaderCache&) = delete;
  ShaderCache& operator=(const ShaderCache&) = delete;
  ShaderCache& operator=(const ShaderCache&&) = delete;

  ShaderCache();
  ~ShaderCache();
  ShaderCache(ShaderCache&&);

  Id idForName(const QString& name);
  gl::ShaderObject& shaderForId(Id id);

private:
  QHash<QString, ShaderFactory*> _shaderFactories;
  QHash<QString, Id> _idForName;
  QVector<gl::ShaderObject*> _allShaderObjects;
};


gl::ShaderObject& ShaderCache::shaderForId(Id id)
{
  return *_allShaderObjects[int(id)];
}


} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_SHADERCACHE_H
