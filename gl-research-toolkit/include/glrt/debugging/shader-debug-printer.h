#ifndef GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
#define GLRT_DEBUGGING_SHADERDEBUGPRINTER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/shaderobject.hpp>
#include <glrt/gui/anttweakbar.h>
#include <glrt/debugging/visualization-renderer.h>

namespace glrt {
namespace debugging {

class ShaderDebugPrinter final
{
public:
  gui::TweakBarCBVar<bool> guiToggle;

  ShaderDebugPrinter();
  ~ShaderDebugPrinter();

  bool handleEvents(const SDL_Event& event);

  void begin();
  void end();

  void draw();

private:
  struct Chunk;
  struct WholeBuffer;

  bool active = false;
  bool mouse_is_pressed = false;
  glm::ivec2 mouseCoordinate;
  gl::ShaderObject shader;
  gl::Buffer counterBuffer;
  gl::Buffer buffer;

  QVector<glm::vec3> positionsToDebug;
  QVector<Arrow> directionsToDebug;

  VisualizationRenderer positionVisualization;
  VisualizationRenderer directionVisualization;

  void setMouseCoordinate(const glm::ivec2& mouseCoordinate, Uint32 windowId);

  template<typename T, typename T_in>
  void printVectorChunk(int dimension, const char* scalarName, const char* vectorPrefix, const glm::tvec4<T_in>& input, bool visualize);
  void printRay(const glm::vec3& origin, const glm::vec3& direction, bool visualize);
  void printChunk(const Chunk& chunk);
};

} // namespace debugging
} // namespace glrt

#endif // GLRT_DEBUGGING_SHADERDEBUGPRINTER_H
