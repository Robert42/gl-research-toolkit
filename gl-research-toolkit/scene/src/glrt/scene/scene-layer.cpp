#include <glrt/scene/scene-layer.h>
#include <glrt/scene/resources/static-mesh-loader.h>
#include <glrt/scene/resources/resource-manager.h>
#include <glrt/scene/resources/resource-index.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;


inline Scene* get_scene(SceneLayer* sceneLayer)
{
  return &sceneLayer->scene;
}

inline resources::ResourceIndex* get_index(SceneLayer* sceneLayer)
{
  return &sceneLayer->index;
}

// -------- SceneLayer ---------------------------------------------------------

/*!
\warning The scene takes ownership over the layer
 */
SceneLayer::SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene)
  : QObject(&scene),
    uuid(uuid),
    scene(scene),
    index(Uuid<resources::ResourceIndex>(QUuid::createUuidV5(QUuid(uuid), QString("index for scene-layer"))))
{
  if(scene._layers.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  scene._layers[uuid] = this;
}

SceneLayer::~SceneLayer()
{
  scene._layers.remove(uuid);

  // #TODO: delete all nodes
}

QList<Node*> SceneLayer::allNodes() const
{
  return _nodes.values();
}

void SceneLayer::loadSceneGraph(const std::string& filename)
{
  SPLASHSCREEN_MESSAGE("Loading Scene-Graph");

  std::string absoluteFilename = QDir::current().absoluteFilePath(QString::fromStdString(filename)).toStdString();

  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING | AngelScriptIntegration::ACCESS_MASK_GLM;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, absoluteFilename.c_str(), "void main(SceneLayer@ sceneLayer)", "scene-graph-file", config, this);
}

void SceneLayer::registerAngelScriptAPIDeclarations()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("SceneLayer", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("SceneLayer", false);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void SceneLayer::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectMethod("SceneLayer", "void loadSceneGraph(const string &in file)", AngelScript::asMETHOD(SceneLayer,loadSceneGraph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("SceneLayer", "Scene@ get_scene()", AngelScript::asFUNCTION(get_scene), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);
  r = angelScriptEngine->RegisterObjectMethod("SceneLayer", "ResourceIndex@ get_index()", AngelScript::asFUNCTION(get_index), AngelScript::asCALL_CDECL_OBJFIRST); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

