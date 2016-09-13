#include <glrt/renderer/toolkit/glsl-macro-wrapper.h>

namespace glrt {
namespace renderer {

const size_t _glslMacroWrapperInterface_capacity = 64;
extern size_t _glslMacroWrapperInterface_index;
extern GLSLMacroWrapperInterface* _glslMacroWrapperInterface[_glslMacroWrapperInterface_capacity];

size_t _glslMacroWrapperInterface_index = 0;
GLSLMacroWrapperInterface* _glslMacroWrapperInterface[_glslMacroWrapperInterface_capacity];

GLSLMacroWrapperInterface::GLSLMacroWrapperInterface()
{
  Q_ASSERT(_glslMacroWrapperInterface_index<_glslMacroWrapperInterface_capacity);
  _glslMacroWrapperInterface[_glslMacroWrapperInterface_index++] = this;
}

GLSLMacroWrapperInterface::~GLSLMacroWrapperInterface()
{
  for(size_t i=0; i<_glslMacroWrapperInterface_index; ++i)
  {
    GLSLMacroWrapperInterface* interface  = _glslMacroWrapperInterface[i];
    if(interface == this)
      interface = nullptr;
  }
}

void GLSLMacroWrapperInterface::initAll()
{
  for(size_t i=0; i<_glslMacroWrapperInterface_index; ++i)
  {
    GLSLMacroWrapperInterface* interface  = _glslMacroWrapperInterface[i];
    if(interface)
      interface->init();
  }
}

} // namespace renderer
} // namespace glrt
