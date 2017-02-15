#include <glrt/sample-application.h>
#include <glrt/toolkit/profiler.h>
#include <glrt/renderer/forward-renderer.h>
#include <glrt/renderer/deferred-renderer.h>


namespace glrt {

SampleApplication::SampleApplication(int& argc, char** argv,
                                     const gui::AntTweakBar::Settings& guiSettings,
                                     const SampleApplication::Settings& sampleApplicationSettings,
                                     const Application::Settings& applicationSettings,
                                     const System::Settings& systemSettings)
  : Application(argc, argv, systemSettings, applicationSettings),
    firstSceneToLoad(sampleApplicationSettings.sceneToLoad),
    scene(&resourceManager),
    voxelizedScene(scene),
    antweakbar(this, guiSettings)
{
  if(sampleApplicationSettings.deferredRenderer)
    renderer = new renderer::DeferredRenderer(systemSettings.windowSize, &scene, &resourceManager, &shaderDebugPrinter);
  else
    renderer = new renderer::ForwardRenderer(systemSettings.windowSize, &scene, &resourceManager, &shaderDebugPrinter);

  initGui();

  if(sampleApplicationSettings.loadDistanceField)
    voxelizedScene.enabledSignedDistanceFields();

  scene.load(glrt::renderer::SampleResourceManager::emptyScene());
}

SampleApplication::~SampleApplication()
{
  delete renderer;
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
  PROFILE_SCOPE("SampleApplication::update()")
  const float deltaTime = Application::update();
  scene.update(deltaTime);
  antweakbar.update(deltaTime);
  renderer->update(deltaTime);
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
  renderer->render();
}

void SampleApplication::endDrawing()
{
  shaderDebugPrinter.end();
  shaderDebugPrinter.draw();

  antweakbar.draw();
}

float SampleApplication::drawSingleFrame()
{
  SDL_Event event;
  while(pollEvent(&event))
  {
    if(handleEvents(event))
      continue;
  }

  const float deltaTime = update();

  beginDrawing();
  drawScene();


  endDrawing();
  swapWindow();

  return deltaTime;
}

void SampleApplication::initGui()
{
  antweakbar.createDebugSceneBar(renderer);
  antweakbar.createDebugShaderBar(renderer, &shaderDebugPrinter);
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

  scene.load(firstSceneToLoad);
}


} // namespace glrt
