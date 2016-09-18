#include "glsl-macro-wrapper.h"

namespace glrt {
namespace renderer {


template<typename T>
inline GLSLMacroWrapper<T>::GLSLMacroWrapper(const QString& preprocessorDeclaration, T defaultValue)
  : preprocessorDeclaration(preprocessorDeclaration),
    value(defaultValue)
{
}

template<typename T>
inline T GLSLMacroWrapper<T>::get_value()
{
  return this->value;
}

template<typename T>
inline void GLSLMacroWrapper<T>::set_value(T value)
{
  if(this->value != value)
  {
    this->value = value;
    updatePreprocessorBlock();
  }
}

template<typename T>
inline void GLSLMacroWrapper<T>::connectWith(VariableWithCallback<T>* v)
{
  set_value(v->get_value());
  v->callback_functions.push_back([this](T v){this->set_value(v);});
}

template<typename T>
inline void GLSLMacroWrapper<T>::updatePreprocessorBlock()
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
inline void GLSLMacroWrapper<T>::init()
{
  updatePreprocessorBlock();
}


} // namespace renderer
} // namespace glrt
