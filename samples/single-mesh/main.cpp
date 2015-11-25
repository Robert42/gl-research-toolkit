#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/debug-camera.h>
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

  //glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj", false); // TODO: load suzanne again
  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::createCube(glm::vec3(1.f), true, false);
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  QDir shaderDir(GLRT_SHADER_DIR"/test");
  glrt::shader::CustomFactory customFactory(shaderDir.absoluteFilePath("plain-unlit-orange.vs"),
                                            shaderDir.absoluteFilePath("plain-unlit-orange.fs"));

  gl::ShaderObject* shaderObject = customFactory.create("plain-unlit-orange");

  TestUniformBlock u;

  glrt::DebugCamera camera(app.sdlWindow);

  u.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
  u.view_projection = camera.projectionMatrix * camera.viewMatrix;
  u.material_color = glm::vec4(1, 0.5, 0, 1);

  gl::Buffer uniformBlock(sizeof(TestUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, &u);

  //glEnable(GL_CULL_FACE); // TODO uncomment

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      camera.handleEvents(event);
    }

    float deltaTime = app.update();
    camera.update(deltaTime);

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    u.model_matrix = glm::rotate(u.model_matrix, glm::radians(90.f) * deltaTime, glm::vec3(0, 1, 0));
    u.view_projection = camera.projectionMatrix * camera.viewMatrix;
    void* mappedData = uniformBlock.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    *reinterpret_cast<TestUniformBlock*>(mappedData) = u;
    uniformBlock.Unmap();

    shaderObject->Activate();
    shaderObject->BindUBO(uniformBlock, "TestUniformBlock");

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    vertexArrayObject.Bind();
    mesh.bind(vertexArrayObject);
    mesh.draw();
    mesh.resetBinding();
    vertexArrayObject.ResetBinding();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    app.swapWindow();
  }

  delete shaderObject;

  return 0;
}
