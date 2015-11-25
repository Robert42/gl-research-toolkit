#include <glrt/shader/shader-cache.h>
#include <glrt/shader/shader-factory.h>

namespace glrt {
namespace shader {

Cache::Cache()
{
}


Cache::~Cache()
{
  for(gl::ShaderObject* shaderObject : _allShaderObjects)
    delete shaderObject;
  _allShaderObjects.clear();

  for(Factory* shaderFactory : _shaderFactories)
    delete shaderFactory;
  _allShaderObjects.clear();
}


Cache::Cache(Cache&& other)
  : Cache()
{
  _shaderFactories.swap(other._shaderFactories);
  _idForName.swap(other._idForName);
  _allShaderObjects.swap(other._allShaderObjects);
}


Cache::Id Cache::idForName(const QString& name)
{
  if(!_idForName.contains(name))
  {
    if(!_shaderFactories.contains(name))
      throw GLRT_EXCEPTION(QString("Unknown shader name `%0`").arg(name));

    gl::ShaderObject* shader = _shaderFactories[name]->create(name);

    Q_ASSERT(shader != nullptr);

    Id id = Id(_allShaderObjects.length());
    _allShaderObjects.append(shader);
    return id;
  }

  return _idForName[name];
}


} // namespace shader
} // namespace glrt

