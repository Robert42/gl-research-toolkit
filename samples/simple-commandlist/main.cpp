#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>
#include <glhelper/texture2d.hpp>
#include <glhelper/framebufferobject.hpp>

#include <glrt/renderer/gl/command-list.h>
#include <glrt/renderer/gl/command-list-recorder.h>

#define WITH_COMMANDLISTS
//#define BINDLESS
//#define WITH_BINDINGS

#define USE_ATTRIBUTES


int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Simple-Commandlist-Quad"));

  const glm::ivec2 videoResolution = glrt::System::windowSize();
  gl::Texture2D colorFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::RGBA8);
  gl::Texture2D depthFramebufferTexture(videoResolution.x, videoResolution.y, gl::TextureFormat::DEPTH24_STENCIL8);
  gl::FramebufferObject framebuffer(gl::FramebufferObject::Attachment(&colorFramebufferTexture), gl::FramebufferObject::Attachment(&depthFramebufferTexture), true);

  const std::vector<float> positions = {0, 1,
                                        0, 0,
                                        1, 0,
                                        1, 1};


  gl::Buffer buffer(8*sizeof(float), gl::Buffer::UsageFlag::IMMUTABLE, positions.data());


  gl::ShaderObject orangeShader("orange-shader");
  const int bindingIndex = 0;
#ifdef USE_ATTRIBUTES
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                   "#version 450 core\n"
                                   "in layout(location=0) vec2 position;\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(position.x, position.y, 0, 1);"
                                   "}\n",
                                   "main.cpp (orange vertex)");
#else
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                   "#version 450 core\n"
                                   "void main()\n"
                                   "{\n"
                                   "gl_Position = vec4(gl_VertexID%1, gl_VertexID%2, 0, 1);"
                                   "}\n",
                                   "main.cpp (orange vertex)");
#endif
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::FRAGMENT,
                                   "#version 450 core\n"
                                   "out vec4 color;\n"
                                   "void main()\n"
                                   "{\n"
                                   "color = vec4(1, 0.5, 0, 1);"
                                   "}\n",
                                   "main.cpp (orange fragment)");
  orangeShader.CreateProgram();

#ifdef WITH_COMMANDLISTS
  gl::CommandList commandList;

  gl::StatusCapture statusCapture;

  framebuffer.Bind(true);
  orangeShader.Activate();
  statusCapture = gl::StatusCapture::capture(gl::StatusCapture::Mode::LINES);
  framebuffer.BindBackBuffer();
  gl::ShaderObject::Deactivate();

  glm::ivec2 tokenRange;
  gl::CommandListRecorder segment;

  segment.beginTokenList();
  segment.append_token_AttributeAddress(bindingIndex, buffer.gpuBufferAddress());
  segment.append_token_DrawArrays(4, 0);
  tokenRange = segment.endTokenList();

  segment.append_drawcall(tokenRange, &statusCapture, &framebuffer);

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
#endif // WITH_BINDINGS

#ifdef BINDLESS
//  glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
  GL_CALL(glVertexAttribFormat, bindingIndex, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float));
  app.showWindow();
  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    framebuffer.Bind(true);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    GL_CALL(glBufferAddressRangeNV, GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, bindingIndex, buffer.gpuBufferAddress(), positions.size()*sizeof(float));

    orangeShader.Activate();
    GL_CALL(glDrawArrays, GL_LINE_LOOP, 0, 4);
    framebuffer.BindBackBuffer();

    framebuffer.BindRead();
    GL_CALL(glBlitFramebuffer, 0, 0, videoResolution.x, videoResolution.y, 0, 0, videoResolution.x, videoResolution.y, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    framebuffer.UnbindRead();

    app.swapWindow();
  }
#endif // BINDLESS

#ifdef WITH_BINDINGS
  gl::VertexArrayObject vertexArray({gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::FLOAT, 2, bindingIndex)});
  app.showWindow();
  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    framebuffer.Bind(true);
    GL_CALL(glClear, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    vertexArray.Bind(); // #TODO: delete line if possible
    buffer.BindVertexBuffer(bindingIndex, 0, vertexArray.GetVertexStride(bindingIndex));

    orangeShader.Activate();
    GL_CALL(glDrawArrays, GL_QUADS, 0, 4);
    framebuffer.BindBackBuffer();

    framebuffer.BindRead();
    GL_CALL(glBlitFramebuffer, 0, 0, videoResolution.x, videoResolution.y, 0, 0, videoResolution.x, videoResolution.y, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT, GL_NEAREST);
    framebuffer.UnbindRead();

    app.swapWindow();
  }
#endif // WITH_BINDINGS

  return 0;
}
