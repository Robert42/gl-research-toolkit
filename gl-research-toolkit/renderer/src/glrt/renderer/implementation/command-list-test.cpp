#include <glrt/renderer/implementation/command-list-test.h>
#include <glrt/renderer/gl/command-list-recorder.h>
#include <glrt/system.h>
#include <glrt/glsl/layout-constants.h>
#include <glrt/toolkit/array.h>

namespace glrt {
namespace renderer {
namespace implementation {


CommandListTest::CommandListTest()
{
}

void CommandListTest::draw()
{
  commandList.call();
}


CommandListTest::AcceptGivenFramebuffer::AcceptGivenFramebuffer(gl::FramebufferObject* framebuffer)
  : framebuffer(framebuffer)
{
}

void CommandListTest::AcceptGivenFramebuffer::captureStateNow(gl::StatusCapture::Mode mode)
{
  framebuffer->Bind(false);
  statusCapture = gl::StatusCapture::capture(mode);
  framebuffer->BindBackBuffer();
  gl::ShaderObject::Deactivate();
}


CommandListTest::SimpleMesh::SimpleMesh(gl::FramebufferObject* framebuffer, const QString& shaderName)
  : AcceptGivenFramebuffer(framebuffer),
    shader(shaderName.toStdString())
{
}

void CommandListTest::SimpleMesh::captureStateNow(gl::StatusCapture::Mode mode)
{
  shader.Activate();
  parent_class::captureStateNow(mode);
  gl::ShaderObject::Deactivate();
}


CommandListTest::SimpleRect::SimpleRect(gl::FramebufferObject* framebuffer, const QString& shaderName, const glm::vec2& rectMin, const glm::vec2& rectMax)
  : SimpleMesh(framebuffer, shaderName)
{
  const Array<float> positions = {rectMin.x, rectMin.y,
                                  rectMin.x, rectMax.y,
                                  rectMax.x, rectMin.y,
                                  rectMax.x, rectMax.y};
  buffer = std::move(gl::Buffer(positions.length()*sizeof(float), gl::Buffer::UsageFlag::IMMUTABLE, positions.data()));
}

void CommandListTest::SimpleRect::captureStateNow(gl::StatusCapture::Mode mode)
{
  GL_CALL(glVertexAttribFormatNV, VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float));
  GL_CALL(glEnableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_POSITION);
  parent_class::captureStateNow(mode);
  GL_CALL(glDisableVertexAttribArray, VERTEX_ATTRIBUTE_LOCATION_POSITION);
}

void CommandListTest::SimpleRect::recordCommands()
{
  glm::ivec2 tokenRange;
  gl::CommandListRecorder recorder;

  const glm::ivec2 videoResolution = glrt::System::windowSize();

  recorder.beginTokenList();
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_AttributeAddress(VERTEX_ATTRIBUTE_LOCATION_POSITION, buffer.gpuBufferAddress());
  recorder.append_token_DrawArrays(4, 0, gl::CommandListRecorder::Strip::STRIP);
  tokenRange = recorder.endTokenList();

  recorder.append_drawcall(tokenRange, &statusCapture, framebuffer);

  this->commandList = gl::CommandListRecorder::compile(std::move(recorder));
}

CommandListTest::OrangeFullscreenRect::OrangeFullscreenRect(gl::FramebufferObject* framebuffer)
  : SimpleRect(framebuffer, "orange-fullscreen-rect")
{
  shader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                             QString("#version 450 core\n"
                             "in layout(location=%0) vec2 position;\n"
                             "void main()\n"
                             "{\n"
                             "gl_Position = vec4(position.x, position.y, 0, 1);"
                             "}\n").arg(VERTEX_ATTRIBUTE_LOCATION_POSITION).toStdString(),
                             "main.cpp (orange vertex)");
  shader.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                             "#version 450 core\n"
                             "out vec4 color;\n"
                             "void main()\n"
                             "{\n"
                             "color = vec4(1, 0.5, 0, 1);"
                             "}\n",
                             "main.cpp (orange fragment)");
  shader.CreateProgram();
}


} // namespace implementation
} // namespace renderer
} // namespace glrt
