#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/shader/shader-factory.h>

#include <glhelper/gl.hpp>


struct TestUniformBlock
{
  glm::mat4 model_matrix;
  glm::mat4 view_projection;
  glm::vec4 material_color;
};

int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Single Mesh"));

  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  QDir shaderDir(GLRT_SHADER_DIR"/test");
  glrt::shader::CustomFactory customFactory(shaderDir.absoluteFilePath("plain-unlit-orange.vs"),
                                            shaderDir.absoluteFilePath("plain-unlit-orange.fs"));

  gl::ShaderObject* shaderObject = customFactory.create("plain-unlit-orange");

  TestUniformBlock u;

  u.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
  u.view_projection = glm::perspectiveFov(90.f, 640.f, 480.f, 0.001f, 100.f ) * glm::translate(glm::mat4(1), glm::vec3(0, 0, -5)); // TODO use the real window size
  u.material_color = glm::vec4(1, 0.5, 0, 1);

  gl::Buffer uniformBlock(sizeof(TestUniformBlock), gl::Buffer::UsageFlag::IMMUTABLE, &u);

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    shaderObject->Activate();
    shaderObject->BindUBO(uniformBlock, "TestUniformBlock");

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
