#include <glrt/renderer/toolkit/compute-shader-set.h>
#include <glrt/renderer/toolkit/shader-compiler.h>
#include <glrt/system.h>

namespace glrt {
namespace renderer {


ComputeShaderSet::ComputeShaderSet(const QString& name, const QString& shaderFileName, const T_MapSize& mapTotalSizeToWorkerGroupSize)
  : name(name),
    mapTotalSizeToWorkerGroupSize(mapTotalSizeToWorkerGroupSize)
{
  QFileInfo fileInfo(shaderFileName);

  shaderFileDir = fileInfo.absoluteDir();
  shaderFileBasename = fileInfo.baseName();
}

ComputeShaderSet::~ComputeShaderSet()
{
}

void ComputeShaderSet::execute(const glm::ivec3& workAmount, const QSet<QString>& preprocessorBlock)
{
  glm::ivec3 groupSize = mapTotalSizeToWorkerGroupSize(workAmount);

  groupSize = min(groupSize, System::maxComputeWorkGroupSize);

  while(groupSize.x*groupSize.y*groupSize.z > System::maxComputeWorkGroupInvocations && groupSize.z>1)
    groupSize.z--;
  while(groupSize.x*groupSize.y*groupSize.z > System::maxComputeWorkGroupInvocations && groupSize.y>1)
    groupSize.y--;
  while(groupSize.x*groupSize.y*groupSize.z > System::maxComputeWorkGroupInvocations && groupSize.x>1)
    groupSize.x--;

  glm::ivec3 numCalls = (workAmount + groupSize - 1) / groupSize;

  if(any(greaterThan(numCalls, System::maxComputeWorkGroupCount)))
    throw GLRT_EXCEPTION(QString("ComputeShaderSet::execute: maxComputeWorkGroupCount exceeded"));

  QSharedPointer<gl::Program>& glProgram = glPrograms[preprocessorBlock][groupSize];

  if(glProgram.isNull())
  {
    ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
    gl::Program temp = shaderCompiler.compileProgramFromFiles(shaderFileBasename,
                                                              shaderFileDir,
                                                              QStringList({QString("#define GROUPS_SIZE_X %0").arg(groupSize.x),
                                                                           QString("#define GROUPS_SIZE_Y %1").arg(groupSize.y),
                                                                           QString("#define GROUPS_SIZE_Z %2").arg(groupSize.z)})
                                                              + preprocessorBlock.toList());

    glProgram = QSharedPointer<gl::Program>(new gl::Program(std::move(temp)));
  }

  glProgram->use();
  Q_ASSERT(numCalls.x * groupSize.x >= workAmount.x);
  Q_ASSERT(numCalls.y * groupSize.y >= workAmount.y);
  Q_ASSERT(numCalls.z * groupSize.z >= workAmount.z);
  GL_CALL(glDispatchCompute, GLuint(numCalls.x), GLuint(numCalls.y), GLuint(numCalls.z));
  gl::Program::useNone();
}


} // namespace renderer
} // namespace glrt
