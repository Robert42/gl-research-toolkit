#include <glrt/renderer/compute-step.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/system.h>

namespace glrt {
namespace renderer {


inline bool isMultiple(const glm::ivec3& a, const glm::ivec3& b)
{
  return glm::all(glm::equal(a % b, glm::ivec3(0)));
}



ComputeStep::ComputeStep(const QString& shaderFileName)
{
  Q_ASSERT(calcBestWorkGroupSize(glm::ivec3(16, 16, 3*16), true, 1536) == glm::ivec3(16, 16, 6));
  Q_ASSERT(calcBestWorkGroupSize(glm::ivec3(16, 16, 3*16), true, 2048) == glm::ivec3(16, 16, 8));

  QFileInfo fileInfo(shaderFileName);
  shaderFileDir = fileInfo.absoluteDir();
  shaderFileBasename = fileInfo.baseName();
}

void ComputeStep::reinit(const glm::ivec3& totalWorkAmount, bool mustBeMultiple, const QSet<QString>& preprocessorBlock)
{
  reinit(calcBestWorkGroupSize(glm::ivec3(16, 16, 3*16), mustBeMultiple), totalWorkAmount, mustBeMultiple, preprocessorBlock);
}

void ComputeStep::reinit(const glm::ivec3& workerGroupSize, const glm::ivec3& totalWorkAmount, bool mustBeMultiple, const QSet<QString>& preprocessorBlock)
{
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

  glm::ivec3 numInvocations;

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

  ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
  glProgram = shaderCompiler.compileProgramFromFiles(shaderFileBasename,
                                                     shaderFileDir,
                                                     QStringList({QString("#define GROUPS_SIZE_X %0").arg(workerGroupSize.x),
                                                                  QString("#define GROUPS_SIZE_Y %1").arg(workerGroupSize.y),
                                                                  QString("#define GROUPS_SIZE_Z %2").arg(workerGroupSize.z)})
                                                     + preprocessorBlock.toList());
}

glm::ivec3 ComputeStep::calcBestWorkGroupSize(const glm::ivec3& totalWorkAmount, bool mustBeMultiple)
{
  return calcBestWorkGroupSize(totalWorkAmount, mustBeMultiple, System::maxComputeWorkGroupInvocations);
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

} // namespace renderer
} // namespace glrt
