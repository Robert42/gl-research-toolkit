#ifndef GLRT_SCENE_RESOURCES_TEXTURESAMPLER_H
#define GLRT_SCENE_RESOURCES_TEXTURESAMPLER_H

#include <glrt/scene/declarations.h>

#include <glhelper/samplerobject.hpp>

namespace glrt {
namespace scene {
namespace resources {

class TextureSampler final
{
public:
  TextureSampler();

  gl::SamplerObject::Desc description;

  static void registerType();
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTURESAMPLER_H
