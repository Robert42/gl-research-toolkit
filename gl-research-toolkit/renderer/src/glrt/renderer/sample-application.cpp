#include <glrt/sample-application.h>


namespace glrt {

SampleApplication::SampleApplication(int& argc, char** argv,
                                     const gui::AntTweakBar::Settings& guiSettings,
                                     const SampleApplication::Settings& sampleApplicationSettings,
                                     const Application::Settings& applicationSettings,
                                     const System::Settings& systemSettings)
  : Application(argc, argv, systemSettings, applicationSettings),
    scene(&resourceManager),
    renderer(systemSettings.windowSize, &scene, &resourceManager, &shaderDebugPrinter),
    antweakbar(this, guiSettings)
{
  initGui();


  scene.load(sampleApplicationSettings.sceneToLoad);
}

SampleApplication::~SampleApplication()
{
}

bool SampleApplication::handleEvents(const SDL_Event& event)
{
  if(scene.handleEvents(event))
    return true;
  if(antweakbar.handleEvents(event))
    return true;
  if(shaderDebugPrinter.handleEvents(event))
    return true;
  return false;
}

float SampleApplication::update()
{
  const float deltaTime = Application::update();
  scene.update(deltaTime);
  antweakbar.update(deltaTime);
  return deltaTime;
}

void SampleApplication::beginDrawing()
{
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  shaderDebugPrinter.begin();
}

void SampleApplication::drawScene()
{
  renderer.render();
}

void SampleApplication::endDrawing()
{
  shaderDebugPrinter.end();
  shaderDebugPrinter.draw();

  antweakbar.draw();
}

void SampleApplication::initGui()
{
  antweakbar.createDebugSceneBar(&renderer);
  antweakbar.createDebugShaderBar(&shaderDebugPrinter);
  antweakbar.createProfilerBar(&profiler);
}

void SampleApplication::shaderRecompileWorkaround()
{
  // This hack is recompiles all shaders after simulating drawing the scene once.
  // this is an ugly workaround for a problem, were the screen kept black until all shaders were recompiled
  const float deltaTime = 1.f / 60.f;
  scene.update(deltaTime);

  drawScene();

  glrt::renderer::ReloadableShader::reloadAll();
}


} // namespace glrt
