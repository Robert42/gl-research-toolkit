#include <glrt/scene/resources/resource-index.h>

#include <angelscript-integration/call-script.h>

namespace glrt {
namespace scene {
namespace resources {


ResourceIndex::ResourceIndex()
{
}

void ResourceIndex::loadIndex(AngelScript::asIScriptEngine* engine, const QString& filename)
{
  AngelScriptIntegration::callScript<void>(engine, filename.toStdString().c_str(), "void main()", "resource-index");
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
