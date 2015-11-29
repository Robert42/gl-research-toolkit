#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene()
  : plainColorMeshes(gl::ShaderObject("plain-color-materials")),
    texturedMeshes(gl::ShaderObject("textured-meshes")),
    maskedMeshes(gl::ShaderObject("masked-meshes")),
    transparentMeshes(gl::ShaderObject("transparent-meshes"))
{
}

void Scene::render()
{
  plainColorMeshes.render();
  texturedMeshes.render();
  maskedMeshes.render();
  transparentMeshes.render();
}


// ======== MaterialMeshList ===================================================


Scene::MaterialMeshList::MaterialMeshList(gl::ShaderObject&& shaderObject)
  : shaderObject(std::move(shaderObject))
{
  const std::string shaderDir = GLRT_SHADER_DIR"/materials";
  this->shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX, shaderDir + '/' + this->shaderObject.GetName() + ".vs");
  this->shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT, shaderDir + '/' + this->shaderObject.GetName() + ".fs");
}

void Scene::MaterialMeshList::render()
{
  // TODO
}


} // namespace scene
} // namespace glrt

