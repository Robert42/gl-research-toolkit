#ifndef GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
#define GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H

#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/texture-manager.h>
#include <glrt/scene/scene.h>

namespace glrt {
namespace scene {

class Scene;
class SceneLayer;

namespace resources {

class TextureManager;

class ResourceManager : public QObject
{
  Q_OBJECT
public:
  typedef ResourceIndex Index;

  StaticMeshLoader& staticMeshLoader;
  TextureManager& textureManager;

  ResourceManager(StaticMeshLoader* staticMeshLoader, TextureManager* textureManager);
  ResourceManager(ResourceManager&&) = delete;
  void operator=(const ResourceManager&) = delete;
  void operator=(ResourceManager&&) = delete;

  virtual ~ResourceManager();

  static ResourceManager* instance();

  static void registerAngelScriptAPIDeclarations();
  static void registerAngelScriptAPI();

  QList<Uuid<Scene>> allRegisteredScenes();

  void loadStaticMesh(const Uuid<StaticMesh>& uuid);
  AABB staticMeshAABB(const Uuid<StaticMesh>& uuid, const AABB& fallback = AABB::invalid()) const;

  QString labelForResourceUuid(const QUuid& uuid, const QString& fallback) const;
  QString labelForResourceUuid(const QUuid& uuid) const;

  LightSource lightSourceForUuid(const Uuid<LightSource>& uuid) const;
  Material materialForUuid(const Uuid<Material>& uuid) const;
  Texture textureForUuid(const Uuid<Texture>& uuid) const;
  TextureSampler defaultTextureSamplerForTextureUuid(const Uuid<Texture>& uuid) const;

  void prepareForGpuBuffer(const Uuid<Material>& uuid) const;

  QString staticMeshFileForUuid(const Uuid<StaticMesh>& uuid, const QString& fallback=QString()) const;
  QString sceneFileForUuid(const Uuid<Scene>& uuid, const QString& fallback=QString()) const;
  QString sceneLayerFileForUuid(const Uuid<SceneLayer>& uuid, const QString& fallback=QString()) const;

  bool isRegistered(const QUuid& uuid) const;

  void foreachIndex(const std::function<bool(const Index* index)>& lambda) const;
  QList<const Index*> allIndices() const;
  const Index* indexForResourceUuid(const QUuid& uuid, const Index* fallback=&Index::fallback) const;

  template<typename T>
  void addMaterialUser(const Uuid<Material>& material, const Uuid<T>& materialuser);

protected:
  virtual bool foreachIndexImpl(const std::function<bool(const Index* index)>& lambda) const = 0;

private:
  static ResourceManager* _singleton;

  friend class glrt::scene::SceneLayer;
  QList<SceneLayer*> _sceneLayers;
};


} // namespace resources
} // namespace scene
} // namespace glrt

#include "resource-manager.inl"

#endif // GLRT_SCENE_RESOURCES_RESOURCEMANAGER_H
