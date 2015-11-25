#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/shader/shader-factory.h>

#include <glhelper/gl.hpp>


namespace Uniforms
{

  struct MeshBlock
  {
    glm::mat4 model_matrix;
  };

  struct SceneBlock
  {
    glm::mat4 view_projection;
  };

  struct MaterialInstance
  {
    glm::vec4 material_color;
  };

}

int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Single Mesh"));

  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  QDir shaderDir(GLRT_SHADER_DIR"/test");
  glrt::shader::CustomFactory customFactory(shaderDir.absoluteFilePath("plain-unlit-orange.vs"),
                                            shaderDir.absoluteFilePath("plain-unlit-orange.fs"));

  gl::ShaderObject* shaderObject = customFactory.create("plain-unlit-orange");

  Uniforms::MeshBlock meshBlock;
  Uniforms::SceneBlock sceneBlock;
  Uniforms::MaterialInstance materialInstanceBlock;

  meshBlock.model_matrix = glm::mat4(1);
  sceneBlock.view_projection = glm::mat4(1);
  materialInstanceBlock.material_color = glm::vec4(1, 0.5, 0, 1);

  gl::Buffer meshUniformBlock(sizeof(Uniforms::MeshBlock), gl::Buffer::UsageFlag::IMMUTABLE, &meshBlock);
  gl::Buffer sceneUniformBlock(sizeof(Uniforms::SceneBlock), gl::Buffer::UsageFlag::IMMUTABLE, &sceneBlock);
  gl::Buffer materialUniformBlock(sizeof(Uniforms::MaterialInstance), gl::Buffer::UsageFlag::IMMUTABLE, &materialInstanceBlock);

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    shaderObject->Activate();
    shaderObject->BindUBO(meshUniformBlock, "MeshBlock");
    shaderObject->BindUBO(sceneUniformBlock, "SceneBlock");
    shaderObject->BindUBO(materialUniformBlock, "MaterialInstance");

    vertexArrayObject.Bind();
    mesh.bind(vertexArrayObject);
    mesh.draw();
    mesh.resetBinding();
    vertexArrayObject.ResetBinding();

    app.swapWindow();
  }

  delete shaderObject;

  return 0;
}
