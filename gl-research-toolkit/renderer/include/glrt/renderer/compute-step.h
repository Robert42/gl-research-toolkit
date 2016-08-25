#ifndef GLRT_RENDERER_COMPUTESTEP_H
#define GLRT_RENDERER_COMPUTESTEP_H

#include <glrt/renderer/dependencies.h>
#include <glrt/renderer/gl/program.h>

namespace glrt {
namespace renderer {

class ComputeStep
{
  Q_DISABLE_COPY(ComputeStep)
public:
  ComputeStep(const QString& shaderFileName);

  void reinit(const glm::ivec3& totalWorkAmount, bool mustBeMultiple=true, const QSet<QString>& preprocessorBlock=QSet<QString>());
  void reinit(const glm::ivec3& workerGroupSize, const glm::ivec3& totalWorkAmount, bool mustBeMultiple=true, const QSet<QString>& preprocessorBlock=QSet<QString>());

  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple);
  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple, int maxNumInvocations);

private:
  glm::ivec3 numInvocations;
  gl::Program glProgram;
  QDir shaderFileDir;
  QString shaderFileBasename;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMPUTESTEP_H
