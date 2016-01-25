#include <glrt/sample-application.h>

namespace glrt {

SampleApplication::SampleApplication(int& argc, char** argv,
                                     const gui::AntTweakBar::Settings& guiSettings,
                                     const SampleApplication::Settings& sampleApplicationSettings,
                                     const Application::Settings& applicationSettings,
                                     const System::Settings& systemSettings)
  : Application(argc, argv, systemSettings, applicationSettings),
    scene(&resourceManager),
    renderer(&scene, &resourceManager),
    antweakbar(this, guiSettings)
{
  initGui();

  scene.load(sampleApplicationSettings.sceneToLoad);
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

void SampleApplication::drawScene()
{
  GL_CALL(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  shaderDebugPrinter.begin();
  renderer.render();
  shaderDebugPrinter.end();
  shaderDebugPrinter.draw();
}

void SampleApplication::drawOverlays()
{
  antweakbar.draw();
}

void SampleApplication::initGui()
{
  antweakbar.createDebugSceneBar(&renderer);
  antweakbar.createDebugShaderBar(&shaderDebugPrinter);
  antweakbar.createProfilerBar(&profiler);
}


} // namespace glrt
