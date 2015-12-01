#include <glrt/scene/scene.h>
#include <glrt/scene/static-mesh-component.h>

#include <glrt/glsl/layout-constants.h>

namespace glrt {
namespace scene {


// ======== Scene ==============================================================


Scene::Scene(SDL_Window* sdlWindow)
  : camera(sdlWindow),
    _cachedStaticStructureCacheIndex(0)
{
}

Scene::~Scene()
{
  QSet<Entity*> entities;
  entities.swap(this->_entities);

  for(Entity* entity : entities)
    delete entity;
}


bool Scene::handleEvents(const SDL_Event& event)
{
  return camera.handleEvents(event);
}


void Scene::update(float deltaTime)
{
  camera.update(deltaTime);
}


void Scene::staticMeshStructureChanged()
{
  ++_cachedStaticStructureCacheIndex;
}


void Scene::AddEntity(Entity* entity)
{
  _entities.insert(entity);
}

void Scene::RemoveEntity(Entity*entity)
{
  _entities.remove(entity);
}


} // namespace scene
} // namespace glrt

