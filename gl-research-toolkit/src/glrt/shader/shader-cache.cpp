#include <glrt/shader/shader-cache.h>
#include <glrt/shader/shader-factory.h>

namespace glrt {
namespace shader {

ShaderCache::ShaderCache()
{
}


ShaderCache::~ShaderCache()
{
  for(gl::ShaderObject* shaderObject : _allShaderObjects)
    delete shaderObject;
  _allShaderObjects.clear();

  for(ShaderFactory* shaderFactory : _shaderFactories)
    delete shaderFactory;
  _allShaderObjects.clear();
}


ShaderCache::ShaderCache(ShaderCache&& other)
  : ShaderCache()
{
  _shaderFactories.swap(other._shaderFactories);
  _idForName.swap(other._idForName);
  _allShaderObjects.swap(other._allShaderObjects);
}


ShaderCache::Id ShaderCache::idForName(const QString& name)
{
  if(!_idForName.contains(name))
  {
    if(!_shaderFactories.contains(name))
      throw GLRT_EXCEPTION(QString("Unknown shader name `%0`").arg(name));

    gl::ShaderObject* shader = _shaderFactories[name]->create();

    Q_ASSERT(shader != nullptr);

    Id id = Id(_allShaderObjects.length());
    _allShaderObjects.append(shader);
    return id;
  }

  return _idForName[name];
}


} // namespace shader
} // namespace glrt

