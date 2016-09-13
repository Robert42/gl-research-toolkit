#ifndef GLRT_RENDERER_GLSLMACROWRAPPER_H
#define GLRT_RENDERER_GLSLMACROWRAPPER_H

#include <glrt/renderer/toolkit/reloadable-shader.h>

namespace glrt {
namespace renderer {

class GLSLMacroWrapperInterface
{
public:
  Q_DISABLE_COPY(GLSLMacroWrapperInterface)

  GLSLMacroWrapperInterface();
  virtual ~GLSLMacroWrapperInterface();

  static void initAll();

protected:
  virtual void init() = 0;
};

template<typename T>
class GLSLMacroWrapper final : public GLSLMacroWrapperInterface
{
public:
  GLSLMacroWrapper(const QString& preprocessorDeclaration, T defaultValue);

  T get_value();
  void set_value(T value);

private:
  const QString preprocessorDeclaration;
  T value;
  QString preprocessorBlock;

  void updatePreprocessorBlock();

  void init() override;
};

} // namespace renderer
} // namespace glrt

#include "glsl-macro-wrapper.inl"

#endif // GLRT_RENDERER_GLSLMACROWRAPPER_H
