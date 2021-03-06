#include <glrt/renderer/compute-step.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/system.h>

namespace glrt {
namespace renderer {


inline bool isMultiple(const glm::ivec3& a, const glm::ivec3& b)
{
  return glm::all(glm::equal(a % b, glm::ivec3(0)));
}



ComputeStep::ComputeStep(const QString& shaderFileName, const glm::ivec3& totalWorkAmount, const QSet<QString>& preprocessorBlock)
  : numInvocations(glm::uninitialize),
    shader(QFileInfo(shaderFileName).baseName(), QFileInfo(shaderFileName).absoluteDir(), preprocessorBlock | groupSizeAsMacro(totalWorkAmount, true))
{
  Q_ASSERT(calcBestWorkGroupSize(glm::ivec3(16, 16, 3*16), true, 1536) == glm::ivec3(16, 16, 6));
  Q_ASSERT(calcBestWorkGroupSize(glm::ivec3(16, 16, 3*16), true, 2048) == glm::ivec3(16, 16, 8));

  Q_ASSERT(numInvocations.x > 0);
  Q_ASSERT(numInvocations.y > 0);
  Q_ASSERT(numInvocations.z > 0);
}

void ComputeStep::invoke()
{
  shader.glProgram.use();
  GL_CALL(glDispatchCompute, GLuint(numInvocations.x), GLuint(numInvocations.y), GLuint(numInvocations.z));
  gl::Program::useNone();
}

QSet<QString> ComputeStep::groupSizeAsMacro(const glm::ivec3& totalWorkAmount, bool mustBeMultiple)
{
  glm::ivec3 workerGroupSize = calcBestWorkGroupSize(totalWorkAmount, mustBeMultiple);

  if(Q_UNLIKELY(glm::any(glm::lessThanEqual(System::maxComputeWorkGroupSize, workerGroupSize))))
  {
    qWarning() << "Too large workerGroupSize"<<workerGroupSize<<"max supported size is"<<System::maxComputeWorkGroupSize;
    Q_UNREACHABLE();
  }

  int numInvokations = workerGroupSize.x*workerGroupSize.y*workerGroupSize.z;
  if(Q_UNLIKELY(numInvokations > System::maxComputeWorkGroupInvocations))
  {
    qWarning() << "Too large workerGroupSize"<<workerGroupSize<<"resulting in"<<numInvokations<<"invokations"<<"max supported num invocations is"<<System::maxComputeWorkGroupInvocations;
    Q_UNREACHABLE();
  }

  if(Q_LIKELY(mustBeMultiple))
  {
    if(Q_UNLIKELY(!isMultiple(totalWorkAmount, workerGroupSize)))
    {
      qWarning() << "totalWorkAmount"<<totalWorkAmount<<"must be a multiple of"<<workerGroupSize;
      Q_UNREACHABLE();
    }

    numInvocations = totalWorkAmount / workerGroupSize;
  }else
  {
    numInvocations = (totalWorkAmount+workerGroupSize-1) / workerGroupSize;
  }

  Q_ASSERT(numInvocations.x > 0);
  Q_ASSERT(numInvocations.y > 0);
  Q_ASSERT(numInvocations.z > 0);
  Q_ASSERT(numInvocations.x * workerGroupSize.x >= totalWorkAmount.x);
  Q_ASSERT(numInvocations.y * workerGroupSize.y >= totalWorkAmount.y);
  Q_ASSERT(numInvocations.z * workerGroupSize.z >= totalWorkAmount.z);

  return QSet<QString>({QString("#define GROUPS_SIZE_X %0").arg(workerGroupSize.x),
                        QString("#define GROUPS_SIZE_Y %1").arg(workerGroupSize.y),
                        QString("#define GROUPS_SIZE_Z %2").arg(workerGroupSize.z)});
}

glm::ivec3 ComputeStep::calcBestWorkGroupSize(const glm::ivec3& workerGroupSize, bool mustBeMultiple)
{
  return calcBestWorkGroupSize(workerGroupSize, mustBeMultiple, System::maxComputeWorkGroupInvocations);
}

glm::ivec3 ComputeStep::calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple, int maxNumInvocations)
{
  glm::ivec3 groupSize = totalWorkAmount;

  if(mustBeMultiple)
  {
    while((!isMultiple(totalWorkAmount, groupSize) || groupSize.x*groupSize.y*groupSize.z > maxNumInvocations) && groupSize.z>1)
      groupSize.z--;
    while((!isMultiple(totalWorkAmount, groupSize) || groupSize.x*groupSize.y*groupSize.z > maxNumInvocations) && groupSize.y>1)
      groupSize.y--;
    while((!isMultiple(totalWorkAmount, groupSize) || groupSize.x*groupSize.y*groupSize.z > maxNumInvocations) && groupSize.x>1)
      groupSize.x--;
  }else
  {
    while(groupSize.x*groupSize.y*groupSize.z > maxNumInvocations && groupSize.z>1)
      groupSize.z--;
    while(groupSize.x*groupSize.y*groupSize.z > maxNumInvocations && groupSize.y>1)
      groupSize.y--;
    while(groupSize.x*groupSize.y*groupSize.z > maxNumInvocations && groupSize.x>1)
      groupSize.x--;
  }

  return groupSize;
}

DynamicComputeStep::DynamicComputeStep(const QString& shaderFileName, const glm::uvec3& typicalWorkAmount, const QSet<QString>& preprocessorBlock)
  : shader(QFileInfo(shaderFileName).baseName(), QFileInfo(shaderFileName).absoluteDir(), preprocessorBlock | groupSizeAsMacro(typicalWorkAmount))
{

}

void DynamicComputeStep::invoke(const glm::uvec3& workAmount)
{
  glm::uvec3 numInvocations = (workAmount+groupSize-glm::uvec3(1))/groupSize;
  shader.glProgram.use();
  GL_CALL(glDispatchCompute, numInvocations.x, numInvocations.y, numInvocations.z);
  gl::Program::useNone();
}

QSet<QString> DynamicComputeStep::groupSizeAsMacro(const glm::uvec3& totalWorkAmount)
{
  const bool mustBeMultiple = false;
  glm::uvec3 workerGroupSize = ComputeStep::calcBestWorkGroupSize(totalWorkAmount, mustBeMultiple);

  this->groupSize = workerGroupSize;

  return QSet<QString>({QString("#define GROUPS_SIZE_X %0").arg(workerGroupSize.x),
                        QString("#define GROUPS_SIZE_Y %1").arg(workerGroupSize.y),
                        QString("#define GROUPS_SIZE_Z %2").arg(workerGroupSize.z)});
}

} // namespace renderer
} // namespace glrt
