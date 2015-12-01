#include <glrt/scene/scene-renderer.h>

namespace glrt {
namespace scene {


Renderer::Renderer(Scene& scene)
  : scene(scene)
{

}

Renderer::~Renderer()
{
}


void Renderer::updateCache()
{
  if(_cachedStaticStructureCacheIndex == scene.cachedStaticStructureCacheIndex)
    return;
}


// ======== Pass ===============================================================


Renderer::Pass::Pass(gl::ShaderObject&& shaderObject)
  : shaderObject(std::move(shaderObject))
{
}

Renderer::Pass::Pass(const QString& materialName, const QStringList& preprocessorMacros)
  : Pass(std::move(gl::ShaderObject(materialName.toStdString())))
{
  std::string preprocessor_definitions = preprocessorMacros.join('\n').toStdString();

  const QDir shaderDir(GLRT_SHADER_DIR"/materials");

  QMap<QString, gl::ShaderObject::ShaderType> shaderTypes;
  shaderTypes[".vs"] = gl::ShaderObject::ShaderType::VERTEX;
  shaderTypes[".fs"] = gl::ShaderObject::ShaderType::FRAGMENT;

  for(const QString& extension : shaderTypes.keys())
  {
    QFileInfo file = shaderDir.filePath(materialName + extension);

    if(!file.exists())
      continue;

    gl::ShaderObject::ShaderType type = shaderTypes[extension];

    if(!preprocessor_definitions.empty())
      this->shaderObject.AddShaderFromSource(type, preprocessor_definitions, "preprocessor-block");
    this->shaderObject.AddShaderFromFile(type, file.absoluteFilePath().toStdString());
  }
}


} // namespace scene
} // namespace glrt

