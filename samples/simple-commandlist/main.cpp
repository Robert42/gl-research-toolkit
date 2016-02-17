#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>
#include <glhelper/texture2d.hpp>
#include <glhelper/framebufferobject.hpp>

#include <glrt/renderer/gl/command-list.h>
#include <glrt/renderer/gl/command-list-recorder.h>

int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Simple-Commandlist-Quad"));

  const glm::ivec2 videoResolution = glrt::System::windowSize();
  gl::Texture2D colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8);
  gl::Texture2D depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH24_STENCIL8);
  gl::FramebufferObject framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true);

  const std::vector<float> positions = {0, 0,
                                        0, 1,
                                        1, 0,
                                        1, 1};


  gl::Buffer buffer(8*sizeof(float), gl::Buffer::UsageFlag::IMMUTABLE, positions.data());


  gl::ShaderObject orangeShader("orange-shader");
  const int bindingIndex = 0;
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                   "#version 450 core\n"
                                   "in layout(location=0) vec2 position;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(position.x, position.y, 0, 1);"
                                   "}\n",
                                   "main.cpp (orange vertex)");
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                                   "#version 450 core\n"
                                   "out vec4 color;\n"
                                   "void main()\n"
                                   "{\n"
                                   "color = vec4(1, 0.5, 0, 1);"
                                   "}\n",
                                   "main.cpp (orange fragment)");
  orangeShader.CreateProgram();

  GL_CALL(glEnableClientState, GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
  GL_CALL(glEnableVertexAttribArray, bindingIndex);

  gl::StatusCapture statusCapture;

  framebuffer.Bind(true);
  orangeShader.Activate();
  GL_CALL(glVertexAttribFormatNV, bindingIndex, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float));
  statusCapture = gl::StatusCapture::capture(gl::StatusCapture::Mode::TRIANGLES);
  framebuffer.BindBackBuffer();
  gl::ShaderObject::Deactivate();

  glm::ivec2 tokenRange;
  gl::CommandListRecorder segment;

  segment.beginTokenList();
  segment.append_token_AttributeAddress(bindingIndex, buffer.gpuBufferAddress());
  segment.append_token_DrawArrays(4, 0, gl::CommandListRecorder::Strip::STRIP);
  tokenRange = segment.endTokenList();

  segment.append_drawcall(tokenRange, &statusCapture, &framebuffer);

  gl::CommandList commandList;
  commandList = gl::CommandListRecorder::compile(std::move(segment));

  app.showWindow();
  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    framebuffer.Bind(true);
    glClearDepth(1.f);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    commandList.call();
    framebuffer.BindRead();
    framebuffer.BindBackBuffer();

    // workaround, see https://www.opengl.org/discussion_boards/showthread.php/185515-NV_command_list/page2
    glUseProgram(0);
     glBegin(GL_POINTS);
     glVertex3f(0, 0, 0);
     glEnd();

    GL_CALL(glBlitFramebuffer, 0, 0, videoResolution.x, videoResolution.y, 0, 0, videoResolution.x, videoResolution.y, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    framebuffer.UnbindRead();

    app.swapWindow();
  }

  return 0;
}
