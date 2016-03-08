#include <glrt/renderer/implementation/command-list-test.h>
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
  const Array<quint16> indices = {0, 1, 2, 2, 1, 3};
  buffer = std::move(gl::Buffer(positions.length()*sizeof(float), gl::Buffer::UsageFlag::IMMUTABLE, positions.data()));
  indexBuffer = std::move(gl::Buffer(indices.length()*sizeof(quint16), gl::Buffer::UsageFlag::IMMUTABLE, indices.data()));

  useIndices = true;
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
  recordBindingStuff(recorder);

  if(useIndices)
  {
    recorder.append_token_ElementAddress(indexBuffer.gpuBufferAddress(), sizeof(quint16));
    recorder.append_token_DrawElements(6, 0, 0, gl::CommandListRecorder::Strip::NO_STRIP);
  }else
  {
    recorder.append_token_DrawArrays(4, 0, gl::CommandListRecorder::Strip::STRIP);
  }

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

CommandListTest::Orange3dRect::Orange3dRect(gl::FramebufferObject* framebuffer, GLuint64 cameraUniformAddress)
  : SimpleRect(framebuffer, "orange-screen-rect"),
    cameraUniformAddress(cameraUniformAddress)
{
  shader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                             QString("#version 450 core\n"
                             "in layout(location=%0) vec2 position;\n"
                             "\n"
                             "layout(binding=%1, std140) uniform SceneBlock\n"
                             "{\n"
                             "  mat4 view_projection;\n"
                             "}scene;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "gl_Position = scene.view_projection * vec4(position.x, position.y, 0, 1);"
                             "}\n").arg(VERTEX_ATTRIBUTE_LOCATION_POSITION).arg(UNIFORM_BINDING_SCENE_VERTEX_BLOCK).toStdString(),
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

void CommandListTest::Orange3dRect::recordBindingStuff(gl::CommandListRecorder& recorder)
{
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_VERTEX_BLOCK, gl::ShaderObject::ShaderType::VERTEX, cameraUniformAddress);
}


CommandListTest::OrangeStaticMeshVertexBase::OrangeStaticMeshVertexBase(gl::FramebufferObject* framebuffer, GLuint64 cameraUniformAddress)
  : parent_class(framebuffer, "orange-static-mesh-vertex-base"),
    cameraUniformAddress(cameraUniformAddress)
{
  shader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                             QString("#version 450 core\n"
                             "in layout(location=%0) vec3 position;\n"
                             "\n"
                             "layout(binding=%1, std140) uniform SceneBlock\n"
                             "{\n"
                             "  mat4 view_projection;\n"
                             "}scene;\n"
                             "\n"
                             "void main()\n"
                             "{\n"
                             "gl_Position = scene.view_projection * vec4(position.xyz, 1);"
                             "}\n").arg(VERTEX_ATTRIBUTE_LOCATION_POSITION).arg(UNIFORM_BINDING_SCENE_VERTEX_BLOCK).toStdString(),
                             "main.cpp (orange vertex)");
  shader.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                             "#version 450 core\n"
                             "out vec4 color;\n"
                             "void main()\n"
                             "{\n"
                             "color = vec4(1, 0.5, 0, 1);\n"
                             "}\n",
                             "main.cpp (orange fragment)");
  shader.CreateProgram();
}

void CommandListTest::OrangeStaticMeshVertexBase::recordCommands()
{
  glm::ivec2 tokenRange;
  gl::CommandListRecorder recorder;

  const glm::ivec2 videoResolution = glrt::System::windowSize();

  recorder.beginTokenList();
  recorder.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  recorder.append_token_UniformAddress(UNIFORM_BINDING_SCENE_VERTEX_BLOCK, gl::ShaderObject::ShaderType::VERTEX, cameraUniformAddress);

  recordBindingStuff(recorder);

  tokenRange = recorder.endTokenList();

  recorder.append_drawcall(tokenRange, &statusCapture, framebuffer);

  this->commandList = gl::CommandListRecorder::compile(std::move(recorder));
}


CommandListTest::OrangeStaticMesh::OrangeStaticMesh(gl::FramebufferObject* framebuffer, GLuint64 cameraUniformAddress, const StaticMeshBuffer* staticMeshBuffer)
  : parent_class(framebuffer, cameraUniformAddress),
    staticMeshBuffer(staticMeshBuffer)
{
}

void CommandListTest::OrangeStaticMesh::captureStateNow(gl::StatusCapture::Mode mode)
{
  StaticMeshBuffer::enableVertexArrays();
  parent_class::captureStateNow(mode);
  StaticMeshBuffer::disableVertexArrays();
}

void CommandListTest::OrangeStaticMesh::recordBindingStuff(gl::CommandListRecorder& recorder)
{
  staticMeshBuffer->recordBind(recorder);
  staticMeshBuffer->recordDraw(recorder);
}


} // namespace implementation
} // namespace renderer
} // namespace glrt
