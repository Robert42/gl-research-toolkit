#include <glrt/renderer/debugging/debug-renderer.h>

namespace glrt {
namespace renderer {
namespace debugging {

DebugRenderer::DebugRenderer(scene::Scene* scene)
  : _scene(scene)
{
  guiToggle.getter = std::bind(&DebugRenderer::isEnabled, this);
  guiToggle.setter = std::bind(&DebugRenderer::setEnabled, this, std::placeholders::_1);

  if(scene)
    loadSceneConnection = QObject::connect(scene, &scene::Scene::sceneLoaded, std::bind(&DebugRenderer::reinit, this));
}

DebugRenderer::DebugRenderer(const DebugRenderer& other)
  : DebugRenderer(other._scene)
{
  if(other.isEnabled())
    this->setEnabled(true);
}

DebugRenderer::DebugRenderer(DebugRenderer&& other)
  : DebugRenderer(other._scene)
{
  if(other.isEnabled())
    this->setEnabled(true);
}

DebugRenderer::~DebugRenderer()
{
  QObject::disconnect(loadSceneConnection);
}


void DebugRenderer::setEnabled(bool enabled)
{
  if(enabled == this->isEnabled())
    return;

  this->_enabled = enabled;

  reinit();
}

bool DebugRenderer::isEnabled() const
{
  return this->_enabled;
}

scene::Scene* DebugRenderer::scene() const
{
  return _scene;
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
