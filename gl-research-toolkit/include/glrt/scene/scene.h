#ifndef GLRT_SCENE_SCENE_H
#define GLRT_SCENE_SCENE_H

#include <glrt/dependencies.h>
#include <glrt/scene/static-mesh.h>
#include <glrt/scene/material.h>
#include <glrt/debug-camera.h>

#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glhelper/texture2d.hpp>

namespace glrt {
namespace scene {


class Entity;
class StaticMeshComponent;

class Scene final : public QObject
{
  Q_OBJECT
public:
  DebugCamera camera;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;
  Scene& operator=(const Scene&) = delete;
  Scene& operator=(Scene&&) = delete;

  Scene(SDL_Window* sdlWindow);
  ~Scene();

  bool handleEvents(const SDL_Event& event);
  void update(float deltaTime);

  void bindSceneUniformBlock();

  void staticMeshStructureChanged();

private:
  friend class Renderer;
  friend class Entity;
  quint64 _cachedStaticStructureCacheIndex;

  QSet<Entity*> _entities;

  void AddEntity(Entity* entity);
  void RemoveEntity(Entity* entity);
};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_SCENE_H
