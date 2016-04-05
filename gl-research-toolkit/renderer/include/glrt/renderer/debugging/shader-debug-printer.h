#ifndef GLRT_RENDERER_DEBUGGING_SHADERDEBUGPRINTER_H
#define GLRT_RENDERER_DEBUGGING_SHADERDEBUGPRINTER_H

#include <glrt/dependencies.h>

#include <glhelper/buffer.hpp>
#include <glhelper/shaderobject.hpp>
#include <glrt/gui/anttweakbar.h>
#include <glrt/renderer/debugging/visualization-renderer.h>
#include <glrt/renderer/gl/command-list-recorder.h>

namespace glrt {
namespace renderer {
namespace debugging {

class ShaderDebugPrinter final
{
public:
  gui::TweakBarCBVar<bool> guiToggle;
  bool clearScene;

  ShaderDebugPrinter();
  ~ShaderDebugPrinter();

  bool handleEvents(const SDL_Event& event);

  void begin();
  void end();

  void recordBinding(gl::CommandListRecorder& recorder);

  void draw();

private:
  struct Chunk;
  struct Header;

  bool active = false;
  bool mouse_is_pressed = false;
  glm::ivec2 mouseCoordinate;
  gl::ShaderObject shader;
  gl::Buffer headerBuffer;
  gl::Buffer chunkBuffer;

  QVector<glm::vec3> positionsToDebug;
  QVector<Arrow> directionsToDebug;

  DebugRenderer positionVisualization;
  DebugRenderer directionVisualization;
  DebugRenderer::List renderList;

  void setMouseCoordinate(const glm::ivec2& mouseCoordinate, Uint32 windowId);

  template<typename T, typename T_in>
  void printVectorChunk(int dimension, const char* scalarName, const char* vectorPrefix, const glm::tvec4<T_in>& input, bool visualize=false);
  void printUint64(const glm::ivec4& input);
  void printRay(const glm::vec3& origin, const glm::vec3& direction, bool visualize);
  void printChunk(const Chunk& chunk);
};

} // namespace debugging
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_DEBUGGING_SHADERDEBUGPRINTER_H
