#include <glrt/renderer/debugging/debug-renderer.h>

namespace glrt {
namespace renderer {
namespace debugging {

DebugRenderer::DebugRenderer(scene::Scene* scene, const ImplementationFactory& factory)
  : _scene(scene),
    _factory(factory)
{
  guiToggle.getter = std::bind(&DebugRenderer::isEnabled, this);
  guiToggle.setter = std::bind(&DebugRenderer::setEnabled, this, std::placeholders::_1);

  if(scene)
    QObject::connect(scene, &scene::Scene::sceneLoaded, std::bind(&DebugRenderer::reinit, this));
}

DebugRenderer::DebugRenderer(const ImplementationFactory& factory)
  : DebugRenderer(nullptr, factory)
{
}

DebugRenderer::DebugRenderer(const DebugRenderer& other)
  : DebugRenderer(other._scene, other._factory)
{
  if(other.isEnabled())
    this->setEnabled(true);
}

DebugRenderer::~DebugRenderer()
{
  deleteImplementation();
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

void DebugRenderer::reinit()
{
  if(this->isEnabled())
  {
    createImplementation();
  }else
  {
    deleteImplementation();
  }
}

void DebugRenderer::createImplementation()
{
  deleteImplementation();
  _implementation = _factory();
  if(_implementation)
    debuggingRenderingEnabled(this, _implementation);
}

void DebugRenderer::deleteImplementation()
{
  if(_implementation)
  {
    debuggingRenderingDisabled(this, _implementation);
    delete _implementation;
    _implementation = nullptr;
  }
}


void DebugRenderer::List::connectTo(DebugRenderer* renderer)
{
  connect(renderer, &DebugRenderer::debuggingRenderingEnabled, this, &DebugRenderer::List::debuggingRenderingEnabled);
  connect(renderer, &DebugRenderer::debuggingRenderingDisabled, this, &DebugRenderer::List::debuggingRenderingDisabled);

  if(renderer->_implementation)
    debuggingRenderingEnabled(renderer, renderer->_implementation);
}

void DebugRenderer::List::render()
{
  for(Implementation* i : implementations)
    i->render();
}

void DebugRenderer::List::debuggingRenderingEnabled(DebugRenderer* renderer, Implementation* implementation)
{
  Q_ASSERT(!implementations.contains(implementation));
  Q_ASSERT(!order.contains(renderer));
  Q_ASSERT(!order.values().contains(implementation));

  order[renderer] = implementation;

  implementations = order.values().toVector();

  Q_ASSERT(implementations.contains(implementation));
  Q_ASSERT(order.contains(renderer));
  Q_ASSERT(order.values().contains(implementation));
}

void DebugRenderer::List::debuggingRenderingDisabled(DebugRenderer* renderer, Implementation* implementation)
{
  Q_ASSERT(implementations.contains(implementation));
  Q_ASSERT(order.contains(renderer));
  Q_ASSERT(order.values().contains(implementation));

  order.remove(renderer);

  implementations.removeAll(implementation);

  Q_ASSERT(!implementations.contains(implementation));
  Q_ASSERT(!order.contains(renderer));
  Q_ASSERT(!order.values().contains(implementation));
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
