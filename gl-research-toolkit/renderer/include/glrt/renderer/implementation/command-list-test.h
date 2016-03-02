#ifndef GLRT_RENDERER_COMMANDLISTTEST_H
#define GLRT_RENDERER_COMMANDLISTTEST_H

#include <glrt/dependencies.h>
#include <glhelper/framebufferobject.hpp>
#include <glhelper/shaderobject.hpp>
#include <glhelper/buffer.hpp>
#include <glrt/renderer/gl/command-list.h>
#include <glrt/renderer/gl/status-capture.h>

namespace glrt {
namespace renderer {
namespace implementation {


class CommandListTest : public QObject
{
  Q_OBJECT

public:
  class AcceptGivenFramebuffer;
  class OrangeFullscreenRect;
  class SimpleMesh;
  class SimpleRect;

  typedef QSharedPointer<CommandListTest> Ptr;

  void draw();

protected:
  CommandListTest();

  gl::StatusCapture statusCapture;
  gl::CommandList commandList;
};

class CommandListTest::AcceptGivenFramebuffer : public CommandListTest
{
  Q_OBJECT

public:
  typedef QSharedPointer<AcceptGivenFramebuffer> Ptr;

  gl::FramebufferObject* const framebuffer;

  AcceptGivenFramebuffer(gl::FramebufferObject* framebuffer);

  virtual void captureStateNow(gl::StatusCapture::Mode mode);
  virtual void recordCommands() = 0;
};

class CommandListTest::SimpleMesh : public AcceptGivenFramebuffer
{
  Q_OBJECT
public:
  typedef QSharedPointer<SimpleMesh> Ptr;

  SimpleMesh(gl::FramebufferObject* framebuffer, const QString& shaderName);

  void captureStateNow(gl::StatusCapture::Mode mode) override;

protected:
  gl::ShaderObject shader;

private:
  typedef AcceptGivenFramebuffer parent_class;
};

class CommandListTest::SimpleRect : public SimpleMesh
{
  Q_OBJECT

public:
  typedef QSharedPointer<SimpleRect> Ptr;

  SimpleRect(gl::FramebufferObject* framebuffer, const QString& shaderName, const glm::vec2& rectMin=glm::vec2(-1, -1), const glm::vec2& rectMax=glm::vec2(1, 1));

  void captureStateNow(gl::StatusCapture::Mode mode) final override;
  void recordCommands() final override;

private:
  typedef SimpleMesh parent_class;

  gl::Buffer buffer;
};

class CommandListTest::OrangeFullscreenRect : public SimpleRect
{
  Q_OBJECT

public:
  typedef QSharedPointer<OrangeFullscreenRect> Ptr;

  OrangeFullscreenRect(gl::FramebufferObject* framebuffer);

private:
  typedef SimpleRect parent_class;
};


} // namespace implementation
} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_COMMANDLISTTEST_H
