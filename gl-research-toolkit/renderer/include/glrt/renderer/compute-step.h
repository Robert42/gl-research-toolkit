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
  friend class DynamicComputeStep;

  // UGLY HACK: numInvocationsis initialized while initilaizing the shader member, so the numInvocations member must be initialized before shader
  glm::ivec3 numInvocations;
  ReloadableShader shader;

  QSet<QString> groupSizeAsMacro(const glm::ivec3& totalWorkAmount, bool mustBeMultiple=true);
  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple);
  static glm::ivec3 calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple, int maxNumInvocations);
};

class DynamicComputeStep
{
  Q_DISABLE_COPY(DynamicComputeStep)
public:
  DynamicComputeStep(const QString& shaderFileName, const glm::uvec3& typicalWorkAmount, const QSet<QString>& preprocessorBlock=QSet<QString>());

  void invoke(const glm::uvec3& workAmount);

private:
  glm::uvec3 groupSize;
  ReloadableShader shader;

  QSet<QString> groupSizeAsMacro(const glm::uvec3& totalWorkAmount);
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMPUTESTEP_H
