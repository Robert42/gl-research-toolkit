#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/renderer/gl/shader-type.h>
#include <glrt/gui/anttweakbar.h>
#include <glrt/scene/fps-debug-controller.h>

#include <glhelper/gl.hpp>
#include <glhelper/shaderobject.hpp>

enum class DebuggingMode : int
{
  None,
  PlainColor,
  ShadeNormals,
  ShadeTangents,
  ShadeUV,
  ShowUV,
};

struct TestUniformBlock
{
  glm::mat4 model_matrix;
  glm::mat4 view_projection;
  glm::vec4 material_color;
  glm::vec3 light_direction;
  DebuggingMode debuggingMode;
};

TestUniformBlock u;

bool wireframe = false;
bool backfaceCulling = true;
float rotationSpeed = 5.f;
glm::quat current_orientation;

QDir shaderDir(SHADER_DIR);

void resetOrientation(glm::quat* orientation)
{
  *orientation = glm::quat();
}

int main(int argc, char** argv)
{
  // ======== Init the application ========
  glrt::Application app(argc,
                        argv,
                        glrt::System::Settings::simpleWindow("Single Mesh" // window title
                                                             ),
                        glrt::Application::Settings::techDemo(true));
  glrt::scene::FpsDebugInputHandler fpsController;
  glrt::scene::CameraParameter camera;
  fpsController.frame.orientation = glm::angleAxis(glm::radians(90.f), glm::vec3(1, 0, 0));
  fpsController.frame.position = glm::vec3(0, -5, 0);
  camera.aspect = glrt::System::windowAspectRatio();
  glrt::gui::AntTweakBar antweakbar(&app,
                                    glrt::gui::AntTweakBar::Settings::sampleGui("This Sample shows how to load and display a simple single mesh" // help text of the sample
                                                                                ));
  TwBar* twBar = antweakbar.createCustomBar("Mesh", // The name of the TweakBar
                                            "Toggle various debug options for the mesh view" // helptext of the TweakBar
                                            );
  antweakbar.createProfilerBar(&app.profiler);

  // ======== Setup the Tweak Bar ========
  TwType TwDebuggingModeType = TwDefineEnumFromString("DebuggingMode", "None,PlainColor,ShadeNormals,ShadeTangents,ShadeUV,ShowUV");

  TwAddVarRW(twBar, "Debugging Mode", TwDebuggingModeType, &u.debuggingMode, nullptr);
  TwAddVarRW(twBar, "Wireframes", TW_TYPE_BOOLCPP, &wireframe, "help='Draw the mesh as wireframe?' group=Presentation");
  TwAddVarRW(twBar, "Backface Culling", TW_TYPE_BOOLCPP, &backfaceCulling, "help='Whether to enable/disable backface culling' group=Presentation");
  TwAddVarRW(twBar, "Color", TW_TYPE_COLOR3F, &u.material_color, "help='Color of the material' group=Presentation");
  TwAddVarRW(twBar, "Rotation Speed", TW_TYPE_FLOAT, &rotationSpeed, "help='How hast does the mesh rotate?' min=0 max=360 group=Rotation");
  TwAddVarRW(twBar, "Orientation", TW_TYPE_QUAT4F, &current_orientation, "help='Orientation of the mesh' showval=false group=Rotation" TWEAKBAR_BLENDER_AXIS);
  TwAddVarRW(twBar, "Light Direction", TW_TYPE_DIR3F, &u.light_direction, "help='The direction of a directional light' showval=true" TWEAKBAR_BLENDER_AXIS);
  TwAddButton(twBar, "Reset Orientation", reinterpret_cast<TwButtonCallback>(resetOrientation), &current_orientation, "help='Resets the orientation to the star value' group=Rotation");

  TwDefine("Mesh/Rotation group=Presentation");

  // ======== Setup the Meshes ========
  // The main mesh we want to load
  glrt::renderer::StaticMeshBuffer mesh = glrt::renderer::StaticMeshBuffer::loadMeshFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.mesh");

  // Get the Vertex Format of it
  gl::VertexArrayObject vertexArrayObject = glrt::renderer::StaticMeshBuffer::generateVertexArrayObject();

  // ======== Setup the Shader ========
  // load and compile the shader
  gl::ShaderObject shaderObject("plain-color");
  shaderObject.AddShaderFromFile(gl::ShaderType::VERTEX, shaderDir.absoluteFilePath("plain-color.vs").toStdString());
  shaderObject.AddShaderFromFile(gl::ShaderType::FRAGMENT, shaderDir.absoluteFilePath("plain-color.fs").toStdString());
  shaderObject.CreateProgram();

  // initialize the uniform block with meaningful values
  u.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
  u.view_projection = camera.projectionMatrix() * fpsController.frame.inverse().toMat4();
  u.material_color = glm::vec4(1, 0.5, 0, 1);
  u.light_direction = glm::normalize(glm::vec3(-0.67, 0.14, -0.73));
  u.debuggingMode = DebuggingMode::None;

  // reserve some GPU space for the uniforms
  gl::Buffer uniformBlock(sizeof(TestUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr);

  app.showWindow();

  // ======== Main Loop ========
  glEnable(GL_DEPTH_TEST);
  while(app.isRunning)
  {
    // -------- Event handling --------
    SDL_Event event;
    while(app.pollEvent(&event))
    {
      if(fpsController.handleEvent(event))
        continue;
      if(antweakbar.handleEvents(event))
        continue;

      switch(event.type)
      {
      case SDL_DROPFILE:
      {
        QFileInfo droppedFile = QString::fromUtf8(event.drop.file);
        SDL_free(event.drop.file);

        if(glrt::renderer::StaticMeshBuffer::isValidFileSuffix(droppedFile))
        {
          glrt::renderer::StaticMeshBuffer newMesh = glrt::renderer::StaticMeshBuffer::loadMeshFile(droppedFile.absoluteFilePath());
          std::swap(newMesh, mesh);
        }else
        {
          SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                                   "Couldn't load Mesh",
                                   QString("The given file `%0` couldn't be laoded as mesh.").arg(droppedFile.fileName()).toStdString().c_str(),
                                   app.sdlWindow);
        }
        break;
      }
      default:
        break;
      }
    }

    float deltaTime = app.update();

    fpsController.update(deltaTime);
    antweakbar.update(deltaTime);

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // -------- update the uniform data --------
    current_orientation = current_orientation * glm::angleAxis(glm::radians(rotationSpeed) * deltaTime, glm::vec3(0, 0, 1));
    u.model_matrix = glm::mat4_cast(current_orientation);
    u.view_projection = camera.projectionMatrix() * fpsController.frame.inverse().toMat4();
    // Note: the color gets modified the Tweakbar, search for &u.material_color

    // send the updated uniform to the Graphic device
    void* mappedData = uniformBlock.Map(gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_BUFFER);
    *reinterpret_cast<TestUniformBlock*>(mappedData) = u;
    uniformBlock.Unmap();

    // -------- Render eveything --------

    // update the settings made by the user
    if(backfaceCulling)
      glEnable(GL_CULL_FACE);
    else
      glDisable(GL_CULL_FACE);
    if(wireframe || u.debuggingMode==DebuggingMode::ShowUV)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // activate the shader
    shaderObject.Activate();
    shaderObject.BindUBO(uniformBlock, "TestUniformBlock");

    // bind the vertex array object
    vertexArrayObject.Bind();

    // draw the mesh
    mesh.bind(vertexArrayObject);
    mesh.draw();

    // unbind the vertex array object
    vertexArrayObject.ResetBinding();

    // has no meaning here, but it's a good practive to reset this settings ;)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);

    // -------- Draw the Gui --------
    antweakbar.draw();

    // -------- Swap front and backbuffer --------
    app.swapWindow();
  }

  return 0;
}
