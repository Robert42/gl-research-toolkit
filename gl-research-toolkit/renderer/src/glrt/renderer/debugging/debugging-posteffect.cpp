#include <glrt/renderer/debugging/debugging-posteffect.h>
#include <glrt/renderer/static-mesh-buffer.h>
#include <glrt/system.h>

namespace glrt {
namespace renderer {
namespace debugging {


class DebuggingPosteffect::Renderer : public DebugRenderer::Implementation
{
public:
  gl::StatusCapture statusCapture;
  gl::CommandList commandList;
  bool depthTest;
  padding<byte, 3> _padding;

  Renderer(bool depthTest);

  virtual void activateShader() = 0;

  void recordCommandList();
  void render() override;
};


DebuggingPosteffect::Renderer::Renderer(bool depthTest)
  : depthTest(depthTest)
{
}

void DebuggingPosteffect::Renderer::recordCommandList()
{
  Q_ASSERT(!renderingData.isNull());

  const glm::ivec2 videoResolution = glrt::System::windowSize();
  gl::FramebufferObject& framebuffer = renderingData->framebuffer;
  //glrt::renderer::Renderer& renderer = renderingData->renderer; #TODO use the buffers from the renderer!

  const int bindingIndex = 0;

  framebuffer.Bind(false);
  activateShader();
  GL_CALL(glVertexAttribFormatNV, bindingIndex, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float));
  GL_CALL(glEnableVertexAttribArray, bindingIndex);
  statusCapture = gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES);
  framebuffer.BindBackBuffer();
  gl::ShaderObject::Deactivate();
  GL_CALL(glDisableVertexAttribArray, bindingIndex);


  glm::ivec2 tokenRange;
  gl::CommandListRecorder segment;

  segment.beginTokenList();
  segment.append_token_Viewport(glm::uvec2(0), glm::uvec2(videoResolution));
  segment.append_token_AttributeAddress(bindingIndex, renderingData->vertexBuffer.gpuBufferAddress());
  segment.append_token_DrawArrays(4, 0, gl::CommandListRecorder::Strip::STRIP);
  tokenRange = segment.endTokenList();

  segment.append_drawcall(tokenRange, &statusCapture, &framebuffer);

  commandList = gl::CommandListRecorder::compile(std::move(segment));
}


void DebuggingPosteffect::Renderer::render()
{
  commandList.call();
}


class OrangeScreen : public DebuggingPosteffect::Renderer
{
public:
  gl::ShaderObject orangeShader;

  OrangeScreen(float zValue, float radius, bool depthTest)
    : Renderer(depthTest),
      orangeShader("orange-shader")
  {
    orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                     QString("#version 450 core\n"
                                             "in layout(location=0) vec2 position;\n"
                                             "void main()\n"
                                             "{\n"
                                             "gl_Position = vec4(position.x, position.y, %0, 1);"
                                             "}\n")
                                     .arg(zValue)
                                     .toStdString(),
                                     "main.debugging-posteffect.cpp (orange vertex)");
    orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                                     QString("#version 450 core\n"
                                             "out vec4 color;\n"
                                             "void main()\n"
                                             "{\n"
                                             "color = vec4(1, 0.5, 0, 1);"
                                             "}\n")
                                     .toStdString(),
                                     "main.debugging-posteffect.cpp (orange fragment)");
    orangeShader.CreateProgram();

    recordCommandList();
  }

  void activateShader() override;
};

void OrangeScreen::activateShader()
{
  orangeShader.Activate();
}


DebugRenderer DebuggingPosteffect::orangeScreen(float zValue, float radius, bool depthTest)
{
  padding<byte,3> padding;
  return DebugRenderer::ImplementationFactory([zValue, radius, depthTest, padding](){return new OrangeScreen(zValue, radius, depthTest);});
}

QSharedPointer<DebuggingPosteffect::SharedRenderingData> DebuggingPosteffect::renderingData;

void DebuggingPosteffect::init(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer)
{
  Q_ASSERT(renderingData.isNull());

  renderingData = QSharedPointer<SharedRenderingData>(new SharedRenderingData(framebuffer, renderer));
}

void DebuggingPosteffect::deinit()
{
  renderingData.clear();
}

DebuggingPosteffect::SharedRenderingData::SharedRenderingData(gl::FramebufferObject* framebuffer, glrt::renderer::Renderer* renderer)
  : framebuffer(*framebuffer),
    renderer(*renderer)
{
  Q_ASSERT(framebuffer);
  Q_ASSERT(renderer);

  float min_coord = -1;
  float max_coord = 1;
  const std::vector<float> positions = {min_coord, min_coord,
                                        min_coord, max_coord,
                                        max_coord, min_coord,
                                        max_coord, max_coord};

  vertexBuffer = std::move(gl::Buffer(sizeof(float)*8, gl::Buffer::UsageFlag::IMMUTABLE, positions.data()));

}

DebuggingPosteffect::SharedRenderingData::~SharedRenderingData()
{
}


} // namespace debugging
} // namespace renderer
} // namespace glrt
