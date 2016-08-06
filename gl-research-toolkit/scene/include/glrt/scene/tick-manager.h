#ifndef GLRT_SCENE_TICKMANAGER_H
#define GLRT_SCENE_TICKMANAGER_H

#include <glrt/scene/node.h>

namespace glrt {
namespace scene {

class TickNodeAttribute;

class TickManager final
{
  Q_DISABLE_COPY(TickManager)
public:
  TickManager();
  ~TickManager();

  void tick(float deltaTime);

private:
  friend class TickNodeAttribute;

  Array<TickNodeAttribute*> tickingAttribute;
};

class TickNodeAttribute : public Node::ModularAttribute
{
  Q_DISABLE_COPY(TickNodeAttribute)
public:
  TickNodeAttribute(Node& node, const Uuid<ModularAttribute>& uuid);
  ~TickNodeAttribute();

  virtual void tick(float deltaTime) = 0;
};


} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_TICKMANAGER_H
