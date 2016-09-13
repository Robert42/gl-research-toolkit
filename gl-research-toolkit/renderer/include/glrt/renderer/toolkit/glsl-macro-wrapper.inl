#include "glsl-macro-wrapper.h"

namespace glrt {
namespace renderer {


template<typename T>
GLSLMacroWrapper<T>::GLSLMacroWrapper(const QString& preprocessorDeclaration, T defaultValue)
  : preprocessorDeclaration(preprocessorDeclaration),
    value(defaultValue)
{
}

template<typename T>
T GLSLMacroWrapper<T>::get_value()
{
  return this->value;
}

template<typename T>
void GLSLMacroWrapper<T>::set_value(T value)
{
  if(this->value != value)
  {
    this->value = value;
    updatePreprocessorBlock();
  }
}

template<typename T>
void GLSLMacroWrapper<T>::updatePreprocessorBlock()
{
  QString newProprocessorBlock = preprocessorDeclaration.arg(value);

  if(newProprocessorBlock != preprocessorBlock)
  {
    if(!preprocessorBlock.isEmpty())
      ReloadableShader::globalPreprocessorBlock.remove(preprocessorBlock);
    ReloadableShader::globalPreprocessorBlock.insert(newProprocessorBlock);
    preprocessorBlock = newProprocessorBlock;
    ReloadableShader::reloadAll();
  }
}

template<typename T>
void GLSLMacroWrapper<T>::init()
{
  updatePreprocessorBlock();
}


} // namespace renderer
} // namespace glrt
