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

TestUniformBlock u;

bool wireframe = true;
bool backfaceCulling = true;
bool showPodest = true;
float rotationSpeed = 5.f;
glm::quat current_orientation;

QDir shaderDir(GLRT_SHADER_DIR"/samples/single-mesh");

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
                        glrt::Application::Settings::techDemo("This Sample shows how to load and display a simple single mesh", // help text of the sample
                                                              "Mesh", // The name of the TweakBar
                                                              "Toggle various debug options for the mesh view" // helptext of the
                                                              ));

  // ======== Setup the Tweak Bar ========
  TwAddVarRW(app.appTweakBar, "Wireframes", TW_TYPE_BOOLCPP, &wireframe, "help='Draw the mesh as wireframe?'");
  TwAddVarRW(app.appTweakBar, "Backface Culling", TW_TYPE_BOOLCPP, &backfaceCulling, "help='Whether to enable/disable backface culling'");
  TwAddVarRW(app.appTweakBar, "Show Podest", TW_TYPE_BOOLCPP, &showPodest, "help='Draw The Podest?'");
  TwAddVarRW(app.appTweakBar, "Color", TW_TYPE_COLOR3F, &u.material_color, "help='Color of the material'");
  TwAddVarRW(app.appTweakBar, "Rotation Speed", TW_TYPE_FLOAT, &rotationSpeed, "help='How hast does the mesh rotate?' min=0 max=360");
  TwAddVarRW(app.appTweakBar, "Orientation", TW_TYPE_QUAT4F, &current_orientation, "help='Orientation of the mesh' showval=false " TWEAKBAR_BLENDER_AXIS);
  TwAddButton(app.appTweakBar, "Reset Orientation", reinterpret_cast<TwButtonCallback>(resetOrientation), &current_orientation, "help='Resets the orientation to the star value'");

  // ======== Setup the Meshes ========
  // The main mesh we want to load
  glrt::scene::StaticMesh mesh = glrt::scene::StaticMesh::loadMeshFromFile(GLRT_ASSET_DIR"/common/meshes/suzanne/suzanne.obj");

  // Get the Vertex Format of it
  gl::VertexArrayObject vertexArrayObject = glrt::scene::StaticMesh::generateVertexArrayObject();

  // Just for fun we create a cube as a podest
  glrt::scene::StaticMesh podest = glrt::scene::StaticMesh::createCube(glm::vec3(2.f, 2.f, 0.1f), true, glm::vec3(0, 0, -1.5));

  // ======== Setup the Shader ========
  // load and compile the shader
  gl::ShaderObject shaderObject("plain-color");
  shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::VERTEX, shaderDir.absoluteFilePath("plain-color.vs").toStdString());
  shaderObject.AddShaderFromFile(gl::ShaderObject::ShaderType::FRAGMENT, shaderDir.absoluteFilePath("plain-color.fs").toStdString());
  shaderObject.CreateProgram();

  // initialize the uniform block with meaningful values
  u.model_matrix = glm::translate(glm::mat4(1), glm::vec3(0, 0, 0));
  u.view_projection = app.debugCamera.viewProjectionMatrix;
  u.material_color = glm::vec4(1, 0.5, 0, 1);

  // reserve some GPU space for the uniforms
  gl::Buffer uniformBlock(sizeof(TestUniformBlock), gl::Buffer::UsageFlag::MAP_WRITE, nullptr);

  // ======== Main Loop ========
  while(app.isRunning)
  {
    // -------- Event handling --------
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    float deltaTime = app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    // -------- update the uniform data --------
    current_orientation = current_orientation * glm::angleAxis(glm::radians(rotationSpeed) * deltaTime, glm::vec3(0, 0, 1));
    u.model_matrix = glm::mat4_cast(current_orientation);
    u.view_projection = app.debugCamera.viewProjectionMatrix;
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
    if(wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // activate the shader
    shaderObject.Activate();
    shaderObject.BindUBO(uniformBlock, "TestUniformBlock");

    // bind the vertex array object
    vertexArrayObject.Bind();

    // draw the mesh
    mesh.bind(vertexArrayObject);
    mesh.draw();

    // draw the podest
    if(showPodest)
    {
      podest.bind(vertexArrayObject);
      podest.draw();
    }

    // unbind the vertex array object
    vertexArrayObject.ResetBinding();

    // has no meaning here, but it's a good practive to reset this settings ;)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);

    // -------- Swap front and backbuffer --------
    app.swapWindow();
  }

  return 0;
}
