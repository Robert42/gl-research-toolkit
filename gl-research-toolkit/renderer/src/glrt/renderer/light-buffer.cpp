#include <glrt/glsl/layout-constants.h>

#include <glrt/renderer/scene-renderer.h>

namespace glrt {
namespace renderer {


LightBuffer::LightBuffer(glrt::scene::Scene& scene)
  : sphereAreaShaderStorageBuffer(scene),
    rectAreaShaderStorageBuffer(scene)
{
}

LightBuffer::~LightBuffer()
{
}

void LightBuffer::update()
{
  sphereAreaShaderStorageBuffer.update();
  rectAreaShaderStorageBuffer.update();
}


bool LightBuffer::needRerecording() const
{
  return sphereAreaShaderStorageBuffer.needRerecording() || rectAreaShaderStorageBuffer.needRerecording();
}


void LightBuffer::recordBinding(gl::CommandListRecorder& recorder,
                                GLushort sphereAreaLightBindingIndex,
                                GLushort rectAreaLightBindingIndex)
{
  update();

  sphereAreaShaderStorageBuffer.recordBinding(recorder, sphereAreaLightBindingIndex, gl::ShaderObject::ShaderType::FRAGMENT);
  rectAreaShaderStorageBuffer.recordBinding(recorder, rectAreaLightBindingIndex, gl::ShaderObject::ShaderType::FRAGMENT);
}


void LightBuffer::recordBinding(gl::CommandListRecorder& recorder)
{
  recordBinding(recorder,
                SHADERSTORAGE_BINDING_LIGHTS_SPHEREAREA,
                SHADERSTORAGE_BINDING_LIGHTS_RECTAREA);
}


} // namespace renderer
} // namespace glrt
