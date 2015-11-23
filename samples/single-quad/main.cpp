#include <glrt/application.h>
#include <glrt/gui/toolbar.h>

#include <glhelper/buffer.hpp>
#include <glhelper/vertexarrayobject.hpp>
#include <glhelper/shaderobject.hpp>



int main(int argc, char** argv)
{
  glrt::Application app(argc, argv, glrt::System::Settings::simpleWindow("Single-Quad"));

  const std::vector<float> positions = {0, 1,
                                        0, 0,
                                        1, 0,
                                        1, 1};


  const int bindingIndex = 0;
  gl::Buffer buffer(8*sizeof(float), gl::Buffer::UsageFlag(gl::Buffer::UsageFlag::IMMUTABLE|gl::Buffer::UsageFlag::MAP_READ), positions.data());

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


  while(app.isRunning())
  {
    sf::Event event;
    while(app.pollEvent(event))
    {
    }

    app.update();

    app.beginDraw();

    vertexArray.Bind();
    buffer.BindVertexBuffer(bindingIndex, 0, vertexArray.GetVertexStride(bindingIndex));

    orangeShader.Activate();
    //orangeShader.BindSSBO(buffer, "position");
    GL_CALL(glDrawArrays, GL_POINTS, 0, 1);

    app.endDraw();
  }

  return 0;
}
