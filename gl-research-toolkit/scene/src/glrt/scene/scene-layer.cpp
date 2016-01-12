#include <glrt/scene/scene-layer.h>

namespace glrt {
namespace scene {

using AngelScriptIntegration::AngelScriptCheck;

/*!
\warning The scene takes ownership over the layer
 */
SceneLayer::SceneLayer(const Uuid<SceneLayer>& uuid, Scene& scene)
  : uuid(uuid),
    scene(scene)
{
  if(scene._layers.contains(uuid))
    throw GLRT_EXCEPTION("Same uuid used twice");

  scene._layers[uuid] = this;
}

SceneLayer::~SceneLayer()
{
  scene._layers.remove(uuid);
}

QList<Node*> SceneLayer::allNodes() const
{
  return _nodes.values();
}

void SceneLayer::loadSceneGraph(const std::string& filename)
{
  qInfo() << "loadSceneGraph(" << filename.c_str() << ")";
}

void SceneLayer::registerAngelScriptAPI()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("SceneLayer", 0, AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  glrt::Uuid<void>::registerCustomizedUuidType("SceneLayer", false);

  r = angelScriptEngine->RegisterObjectMethod("SceneLayer", "void loadSceneGraph(const string &in file)", AngelScript::asMETHOD(SceneLayer,loadSceneGraph), AngelScript::asCALL_THISCALL); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}


} // namespace scene
} // namespace glrt

