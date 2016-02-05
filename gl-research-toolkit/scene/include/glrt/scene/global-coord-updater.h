#ifndef GLRT_SCENE_GLOBALCOORDUPDATER_H
#define GLRT_SCENE_GLOBALCOORDUPDATER_H

#include <glrt/toolkit/fragmented-array.h>
#include <glrt/scene/node.h>

#include "implementation/global-coord-array-order.h"

namespace glrt {
namespace scene {

class GlobalCoordUpdater
{
public:
  GlobalCoordUpdater();
};

} // namespace scene
} // namespace glrt

#include "implementation/global-coord-array-order.inl"

#endif // GLRT_SCENE_GLOBALCOORDUPDATER_H
