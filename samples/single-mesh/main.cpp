#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/scene/static-mesh.h>

#include <glhelper/gl.hpp>


struct TestUniformBlock
{
  glm::mat4 model_matrix;
  glm::mat4 view_projection;
  glm::vec4 material_color;
};

int main(int argc, char** argv)
{
  TestUniformBlock u;

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
                                                              "Toggle various debug options for the mesh view" // helptext of the
                                                              ));

  TwAddVarRW(app.appTweakBar, "Wireframes", TW_TYPE_BOOLCPP, &wireframe, "help='Draw the mesh as wireframe?'");
  TwAddVarRW(app.appTweakBar, "Backface Culling", TW_TYPE_BOOLCPP, &backfaceCulling, "help='Whether to enable/disable backface culling'");
  TwAddVarRW(app.appTweakBar, "Show Podest", TW_TYPE_BOOLCPP, &showPodest, "help='Draw The Podest?'");
  TwAddVarRW(app.appTweakBar, "Rotation Speed", TW_TYPE_FLOAT, &rotationSpeed, "help='How hast does the mesh rotate?' min=0 max=360");
  TwAddVarRW(app.appTweakBar, "Color", TW_TYPE_COLOR3F, &u.material_color, "help='Color of the material'");
  // TODO:: also allow modifying the orientation with TweakBar

  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");
  glrt::scene::StaticMesh podest = glrt::scene::StaticMesh::createCube(glm::vec3(2.f, 2.f, 0.1f), true, glm::vec3(0, 0, -1.5));
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  QDir shaderDir(GLRT_SHADER_DIR"/samples/single-mesh");

  gl::ShaderObject shaderObject("plain-color");
  shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX, shaderDir.absoluteFilePath("plain-color.vs").toStdString());
  shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT, shaderDir.absoluteFilePath("plain-color.fs").toStdString());
  shaderObject.CreateProgram();

  u.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
  u.view_projection = app.debugCamera.projectionMatrix * app.debugCamera.viewMatrix;
  u.material_color = glm::vec4(1, 0.5, 0, 1);

  gl::Buffer uniformBlock(sizeof(TestUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, &u);

  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    float deltaTime = app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    u.model_matrix = glm::rotate(u.model_matrix, glm::radians(rotationSpeed) * deltaTime, glm::vec3(0, 0, 1));
    u.view_projection = app.debugCamera.projectionMatrix * app.debugCamera.viewMatrix;
    void* mappedData = uniformBlock.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    *reinterpret_cast<TestUniformBlock*>(mappedData) = u;
    uniformBlock.Unmap();

    shaderObject.Activate();
    shaderObject.BindUBO(uniformBlock, "TestUniformBlock");

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

  return 0;
}
