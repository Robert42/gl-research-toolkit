#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>



int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Simple-Commandlist-Quad"));

  const std::vector<float> positions = {0, 1,
                                        0, 0,
                                        1, 0,
                                        1, 1};


  const int bindingIndex = 0;
  gl::Buffer buffer(8*sizeof(float), gl::Buffer::UsageFlag::IMMUTABLE, positions.data());

  gl::VertexArrayObject vertexArray({gl::VertexArrayObject::Attribute(gl::VertexArrayObject::Attribute::Type::FLOAT, 2, bindingIndex)});

  gl::ShaderObject orangeShader("orange-shader");
  orangeShader.AddShaderFromSource(gl::ShaderObject::ShaderType::VERTEX,
                                   "#version 450 core\n"
                                   "in vec2 position;\n"
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


  app.showWindow();


  while(app.isRunning)
  {
    SDL_Event event;
    while(app.pollEvent(&event))
    {
    }

    app.update();

    GL_CALL(glClear, GL_COLOR_BUFFER_BIT);

    vertexArray.Bind();
    buffer.BindVertexBuffer(bindingIndex, 0, vertexArray.GetVertexStride(bindingIndex));

    orangeShader.Activate();
    GL_CALL(glDrawArrays, GL_QUADS, 0, 4);

    app.swapWindow();
  }

  return 0;
}
