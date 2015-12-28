#ifndef GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
#define GLRT_SCENE_RESOURCES_ASSETCONVERTER_H

#include "resource-uuid.h"

namespace glrt {
namespace scene {
namespace resources {

class AssetConverter
{
public:
  AssetConverter();
  virtual ~AssetConverter();

  static void registerAngelScriptAPI();

  AssetConverter(const AssetConverter&) = delete;
  AssetConverter(AssetConverter&&) = delete;
  AssetConverter&operator=(const AssetConverter&) = delete;
  AssetConverter&operator=(AssetConverter&&) = delete;
};

class StaticMeshConverter : public AssetConverter
{
public:
  void convert(const std::string& sourceFile, const std::string& targetFile, const StaticMeshUuid& uuid);
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_ASSETCONVERTER_H
