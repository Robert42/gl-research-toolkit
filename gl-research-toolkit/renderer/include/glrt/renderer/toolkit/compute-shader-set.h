#ifndef GLRT_RENDERER_COMPUTESHADERSET_H
#define GLRT_RENDERER_COMPUTESHADERSET_H

#include <glrt/renderer/dependencies.h>
#include <glrt/renderer/gl/program.h>

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
  QDir shaderFileDir;
  QString shaderFileBasename;
  T_MapSize mapTotalSizeToWorkerGroupSize;

  QHash<glm::ivec3, QSharedPointer<gl::Program>> glPrograms;

  QSharedPointer<gl::Program> createShaderObject(const glm::ivec3& groupSize);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMPUTESHADERSET_H
