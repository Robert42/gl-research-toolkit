#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/ticking-object.h>
#include <glrt/toolkit/fragmented-array.h>

namespace glrt {
namespace scene {

class TickManager : public QObject
{
  Q_OBJECT
public:
  TickManager(QObject* parent=nullptr);


};

} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_TICKMANAGER_H
