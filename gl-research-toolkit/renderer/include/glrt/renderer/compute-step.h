#ifndef GLRT_RENDERER_COMPUTESTEP_H
#define GLRT_RENDERER_COMPUTESTEP_H

#include <glrt/renderer/dependencies.h>
#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {

class ComputeStep
{
  Q_DISABLE_COPY(ComputeStep)
public:
  ComputeStep(const QString& shaderFileName, const glm::ivec3& totalWorkAmount, const QSet<QString>& preprocessorBlock=QSet<QString>());

  void invoke();


private:
  ReloadableShader shader;
  glm::ivec3 numInvocations;

  QSet<QString> groupSizeAsMacro(const glm::ivec3& totalWorkAmount, bool mustBeMultiple=true);
  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple);
  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple, int maxNumInvocations);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMPUTESTEP_H
