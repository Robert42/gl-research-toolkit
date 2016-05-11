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

void ComputeShaderSet::execute(const glm::ivec3& workAmount)
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

  QSharedPointer<gl::Program> glProgram = glPrograms.value(groupSize);

  if(glProgram.isNull())
  {
    ShaderCompiler& shaderCompiler = ShaderCompiler::singleton();
    gl::Program temp = shaderCompiler.compileProgramFromFiles(shaderFileBasename,
                                                              shaderFileDir,
                                                              QStringList({QString("#define GROUPS_SIZE_X %0\n").arg(groupSize.x),
                                                                           QString("#define GROUPS_SIZE_Y %1\n").arg(groupSize.y),
                                                                           QString("#define GROUPS_SIZE_Z %2\n").arg(groupSize.z)}));

    glProgram = QSharedPointer<gl::Program>(new gl::Program(std::move(temp)));
    glPrograms[groupSize] = glProgram;
  }

  glProgram->use();
  GL_CALL(glDispatchCompute, GLuint(numCalls.x), GLuint(numCalls.y), GLuint(numCalls.z));
  gl::Program::useNone();
}


} // namespace renderer
} // namespace glrt
