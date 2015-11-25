#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/shader/shader-factory.h>

#include <glhelper/gl.hpp>


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Single Mesh"));

  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  QDir shaderDir(GLRT_SHADER_DIR"/test");
  glrt::shader::CustomFactory customFactory(shaderDir.absoluteFilePath("plain-unlit-orange.vs"),
                                            shaderDir.absoluteFilePath("plain-unlit-orange.fs"));

  gl::ShaderObject* shaderObject = customFactory.create("plain-unlit-orange");

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    shaderObject->Activate();

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
