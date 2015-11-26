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
  bool wireframe = true;
  bool backfaceCulling = true;
  bool showPodest = true;
  float rotationSpeed = 5.f;

  glrt::Application app(argc,
                        argv,
                        glrt::System::Settings::simpleWindow("Single Mesh" // window title
                                                             ),
                        glrt::Application::Settings::techDemo("This Sample shows how to load and display a simple single mesh", // help text of the sample
                                                              "Mesh", // The name of the TweakBar
                                                              "Toggle various debug options for the mesh view", // helptext of the
                                                              100 // height of the bar
                                                              ));

  TwAddVarRW(app.appTweakBar, "Wireframes", TW_TYPE_BOOLCPP, &wireframe, "help='Draw the mesh as wireframe?'");
  TwAddVarRW(app.appTweakBar, "Backface Culling", TW_TYPE_BOOLCPP, &backfaceCulling, "help='Whether to enable/disable backface culling'");
  TwAddVarRW(app.appTweakBar, "Show Podest", TW_TYPE_BOOLCPP, &showPodest, "help='Draw The Podest?'");
  TwAddVarRW(app.appTweakBar, "Rotation Speed", TW_TYPE_FLOAT, &rotationSpeed, "help='How hast does the mesh rotate?' min=0 max=360");

  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");
  glrt::scene::StaticMesh podest = glrt::scene::StaticMesh::createCube(glm::vec3(2.f, 2.f, 0.1f), true, glm::vec3(0, 0, -1.5));
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

    u.model_matrix = glm::rotate(u.model_matrix, glm::radians(rotationSpeed) * deltaTime, glm::vec3(0, 0, 1));
    u.view_projection = camera.projectionMatrix * camera.viewMatrix;
    void* mappedData = uniformBlock.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    *reinterpret_cast<TestUniformBlock*>(mappedData) = u;
    uniformBlock.Unmap();

    shaderObject->Activate();
    shaderObject->BindUBO(uniformBlock, "TestUniformBlock");

    if(backfaceCulling)
      glEnable(GL_CULL_FACE);
    else
      glDisable(GL_CULL_FACE);
    if(wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    vertexArrayObject.Bind();
    mesh.bind(vertexArrayObject);
    mesh.draw();
    if(showPodest)
    {
      podest.bind(vertexArrayObject);
      podest.draw();
      podest.resetBinding();
    }
    vertexArrayObject.ResetBinding();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    app.swapWindow();
  }

  delete shaderObject;

  return 0;
}
