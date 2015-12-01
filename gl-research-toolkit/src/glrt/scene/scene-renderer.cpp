#include <glrt/scene/scene-renderer.h>

#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace scene {


Renderer::Renderer(Scene* scene)
  : scene(*scene),
    sceneUniformBuffer(sizeof(SceneUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr),
    staticMeshVertexArrayObject(std::move(StaticMesh::generateVertexArrayObject()))
{
}

Renderer::~Renderer()
{
}


void Renderer::render()
{
  updateSceneUniform();

  sceneUniformBuffer.BindUniformBuffer(UNIFORM_BINDING_SCENE_BLOCK);

  staticMeshVertexArrayObject.Bind();

  renderImplementation();

  staticMeshVertexArrayObject.ResetBinding();
}

void Renderer::updateSceneUniform()
{
  SceneUniformBlock& sceneUniformData =  *reinterpret_cast<SceneUniformBlock*>(sceneUniformBuffer.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER));
  sceneUniformData.view_projection_matrix = scene.camera.viewProjectionMatrix;
  sceneUniformBuffer.Unmap();
}


// ======== Pass ===============================================================


Renderer::Pass::Pass(Renderer* renderer, gl::ShaderObject&& shaderObject)
  : renderer(*renderer),
    shaderObject(std::move(shaderObject))
{
}

Renderer::Pass::Pass(Renderer* renderer, const QString& materialName, const QStringList& preprocessorBlock)
  : Pass(renderer, std::move(gl::ShaderObject(materialName.toStdString())))
{
  std::string preprocessor_definitions = preprocessorBlock.join('\n').toStdString();

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


inline void Renderer::Pass::updateCache()
{
  if(_cachedStaticStructureCacheIndex == renderer.scene._cachedStaticStructureCacheIndex)
    return;
}


void Renderer::Pass::render()
{
  updateCache();
}


} // namespace scene
} // namespace glrt

