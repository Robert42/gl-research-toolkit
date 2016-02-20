#ifndef GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
#define GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H

#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {

class Scene;
class SceneLayer;

namespace resources {


class ResourceManager : public QObject
{
  Q_OBJECT
public:
  typedef ResourceIndex Index;

  StaticMeshLoader& staticMeshLoader;

  ResourceManager(StaticMeshLoader* staticMeshLoader);
  ResourceManager(ResourceManager&&) = delete;
  void operator=(const ResourceManager&) = delete;
  void operator=(ResourceManager&&) = delete;

  virtual ~ResourceManager();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  QList<Uuid<Scene>> allRegisteredScenes();

  QString labelForResourceUuid(const QUuid& uuid, const QString& fallback) const;
  QString labelForResourceUuid(const QUuid& uuid) const;

  LightSource lightSourceForUuid(const Uuid<LightSource>& uuid) const;
  Material materialForUuid(const Uuid<Material>& uuid) const;

  QString sceneFileForUuid(const Uuid<Scene>& uuid, const QString& fallback=QString()) const;
  QString sceneLayerFileForUuid(const Uuid<SceneLayer>& uuid, const QString& fallback=QString()) const;

  void foreachIndex(const std::function<bool(const Index* index)>& lambda) const;
  QList<const Index*> allIndices() const;
  const Index* indexForResourceUuid(const QUuid& uuid, const Index* fallback=&Index::fallback) const;

  template<typename T>
  void addMaterialUser(const Uuid<Material>& material, const Uuid<T>& materialuser);

protected:
  virtual bool foreachIndexImpl(const std::function<bool(const Index* index)>& lambda) const = 0;

private:
  friend class glrt::scene::SceneLayer;
  QList<SceneLayer*> _sceneLayers;

  void foreachWritableIndex(const std::function<bool(Index* index)>& lambda);
  ResourceIndex* writableIndexForResourceUuid(const QUuid& uuid, Index* fallback);
};


} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-manager.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
