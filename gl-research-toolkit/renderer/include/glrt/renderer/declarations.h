#ifndef GLRT_RENDERER_DECLARATIONS_H
#define GLRT_RENDERER_DECLARATIONS_H

#include <glrt/scene/declarations.h>

namespace glrt {
namespace renderer {

using scene::resources::BoundingSphere;

enum class Pass
{
  DEPTH_PREPASS,
  FORWARD_PASS,
};

inline int qHash(Pass pass){return ::qHash(static_cast<int>(pass));}

using glrt::scene::resources::Texture;
using glrt::scene::resources::TextureSampler;
using glrt::scene::resources::TextureManager;
using glrt::scene::resources::StaticMesh;
using glrt::scene::resources::StaticMeshLoader;
using glrt::scene::resources::Material;

class Renderer;

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DECLARATIONS_H
