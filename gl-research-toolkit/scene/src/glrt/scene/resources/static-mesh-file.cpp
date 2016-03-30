#include <glrt/scene/resources/static-mesh-file.h>

namespace glrt {
namespace scene {
namespace resources {

StaticMeshFile::StaticMeshFile()
{
}

quint64 StaticMeshFile::magicNumber()
{
  return magicNumberForString("glrtmesh");
}


} // namespace resources
} // namespace scene
} // namespace glrt
