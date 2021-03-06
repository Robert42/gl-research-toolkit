#include <glrt/application.h>
#include <glrt/gui/toolbar.h>
#include <glrt/renderer/gl/shader-type.h>
#include <gtest/gtest.h>

#include <glhelper/screenalignedtriangle.hpp>
#include <glhelper/shaderobject.hpp>

#include <QElapsedTimer>

const QDir shaderDir(SHADER_DIR);

TEST(shader_includes, shader_includes)
{
  int argc = 0;
  char** argv = nullptr;
  glrt::Application app(argc, argv, glrt::System::Settings::addVSync(glrt::System::Settings::simpleWindow("Shader-Includes")));

  gl::Details::ShaderIncludeDirManager::addIncludeDirs(shaderDir.absoluteFilePath("global"));

  gl::ScreenAlignedTriangle screenAlignedTriangle;

  gl::ShaderObject shader("test-shader");
  shader.AddShaderFromFile(gl::ShaderType::FRAGMENT, shaderDir.absoluteFilePath("test-shader.fs").toStdString());
  shader.CreateProgram();
  shader.Activate();
}
