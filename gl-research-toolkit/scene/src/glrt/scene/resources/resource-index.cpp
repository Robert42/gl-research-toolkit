#include <glrt/scene/resources/resource-index.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {

using AngelScriptIntegration::AngelScriptCheck;


ResourceIndex::ResourceIndex()
{
}

void ResourceIndex::registerAngelScriptFunctions()
{
  int r;
  asDWORD previousMask = angelScriptEngine->SetDefaultAccessMask(ACCESS_MASK_RESOURCE_LOADING);

  r = angelScriptEngine->RegisterObjectType("ResourceIndex", sizeof(ResourceIndex), AngelScript::asOBJ_REF|AngelScript::asOBJ_NOCOUNT); AngelScriptCheck(r);

  angelScriptEngine->SetDefaultAccessMask(previousMask);
}

void ResourceIndex::loadIndex(const QString& filename)
{
  AngelScriptIntegration::ConfigCallScript config;
  config.accessMask = ACCESS_MASK_RESOURCE_LOADING;

  AngelScriptIntegration::callScriptExt<void>(angelScriptEngine, filename.toStdString().c_str(), "void main()", "resource-index", config);
}

State ResourceIndex::stateOf(const QUuid& uuid) const
{
  if(loadedRessources.contains(uuid))
    return State::LOADED;
  else if(loadingRessources.contains(uuid))
    return State::LOADING;
  else if(unloadedRessources.contains(uuid))
    return State::REGISTERED;
  else
    return State::NONE;
}

bool ResourceIndex::isRegistered(const QUuid& uuid) const
{
  return unloadedRessources.contains(uuid)
      || isLoading(uuid)
      || isLoaded(uuid);
}

bool ResourceIndex::isLoading(const QUuid& uuid) const
{
  return loadingRessources.contains(uuid);
}

bool ResourceIndex::isLoaded(const QUuid& uuid) const
{
  return loadedRessources.contains(uuid);
}

bool ResourceIndex::classInvariant()
{
  bool everyRessourceHasOnlyOneState = (unloadedRessources & loadingRessources).isEmpty()
                                    && (loadedRessources   & loadingRessources).isEmpty()
                                    && (loadedRessources   & unloadedRessources).isEmpty();

  return everyRessourceHasOnlyOneState;
}


} // namespace resources
} // namespace glrt
} // namespace scene
