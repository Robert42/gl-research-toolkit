#ifndef GLRT_RENDERER_DEBUGGING_LIGHTVISUALIZATION_H
#define GLRT_RENDERER_DEBUGGING_LIGHTVISUALIZATION_H

#include <glrt/scene/light-component.h>

namespace glrt {
namespace renderer {
namespace debugging {

glrt::scene::Node::Component* createLightDecoration(glrt::scene::LightComponent* l);

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_LIGHTVISUALIZATION_H
