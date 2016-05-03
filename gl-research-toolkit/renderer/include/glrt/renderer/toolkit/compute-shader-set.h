#ifndef GLRT_RENDERER_COMPUTESHADERSET_H
#define GLRT_RENDERER_COMPUTESHADERSET_H

#include <glrt/renderer/dependencies.h>
#include <glhelper/shaderobject.hpp>

namespace glrt {
namespace renderer {

class ComputeShaderSet
{
  Q_DISABLE_COPY(ComputeShaderSet)

public:
  typedef std::function<glm::ivec3(const glm::ivec3&)> T_MapSize;

  ComputeShaderSet(const QString& name, const QString& shaderFileName, const T_MapSize& mapTotalSizeToWorkerGroupSize);
  ~ComputeShaderSet();

  void execute(const glm::ivec3& workAmount);

private:
  QString name;
  QString shaderFileName;
  T_MapSize mapTotalSizeToWorkerGroupSize;

  QHash<glm::ivec3, QSharedPointer<gl::ShaderObject>> shaders;

  QSharedPointer<gl::ShaderObject> createShaderObject(const glm::ivec3& groupSize);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMPUTESHADERSET_H
