#include <glrt/renderer/toolkit/compute-shader-set.h>
#include <glrt/renderer/gl/shader-type.h>
#include <glrt/system.h>

namespace glrt {
namespace renderer {


ComputeShaderSet::ComputeShaderSet(const QString& name, const QString& shaderFileName, const T_MapSize& mapTotalSizeToWorkerGroupSize)
  : name(name),
    shaderFileName(shaderFileName),
    mapTotalSizeToWorkerGroupSize(mapTotalSizeToWorkerGroupSize)
{
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

  QSharedPointer<gl::ShaderObject> shader = shaders.value(groupSize);

  if(shader.isNull())
  {
    shader = QSharedPointer<gl::ShaderObject>(new gl::ShaderObject(QString("%0 [%1,%2,%3]").arg(name).arg(groupSize.x).arg(groupSize.y).arg(groupSize.z).toStdString()));
    shader->AddShaderFromFile(gl::ShaderType::COMPUTE,
                              shaderFileName.toStdString(),
                              QString("#define GROUPS_SIZE_X %0\n"
                                      "#define GROUPS_SIZE_Y %1\n"
                                      "#define GROUPS_SIZE_Z %2\n")
                              .arg(groupSize.x)
                              .arg(groupSize.y)
                              .arg(groupSize.z).toStdString());
    shader->CreateProgram();
    shaders[groupSize] = shader;
  }

  shader->Activate();
  GL_CALL(glDispatchCompute, GLuint(numCalls.x), GLuint(numCalls.y), GLuint(numCalls.z));
  gl::ShaderObject::Deactivate();
}


} // namespace renderer
} // namespace glrt
