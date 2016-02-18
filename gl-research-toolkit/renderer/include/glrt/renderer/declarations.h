#ifndef GLRT_RENDERER_DECLARATIONS_H
#define GLRT_RENDERER_DECLARATIONS_H

#include <glrt/scene/declarations.h>

namespace glrt {
namespace renderer {

enum class Pass
{
  DEPTH_PREPASS,
  FORWARD_PASS,
};

inline int qHash(Pass pass){return ::qHash(static_cast<int>(pass));}

using glrt::scene::resources::StaticMesh;
using glrt::scene::resources::StaticMeshLoader;
using glrt::scene::resources::Material;

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DECLARATIONS_H
