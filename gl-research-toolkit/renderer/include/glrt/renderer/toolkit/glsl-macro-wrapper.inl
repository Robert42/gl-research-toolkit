#include "glsl-macro-wrapper.h"

namespace glrt {
namespace renderer {


template<typename T>
inline GLSLMacroWrapper<T>::GLSLMacroWrapper(const QString& preprocessorDeclaration, T defaultValue)
  : VariableWithCallback<T>(defaultValue),
    preprocessorDeclaration(preprocessorDeclaration)
{
}

template<typename T>
inline void GLSLMacroWrapper<T>::updatePreprocessorBlock()
{
  QString newProprocessorBlock = preprocessorDeclaration.arg(VariableWithCallback<T>::get_value());

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
  VariableWithCallback<T>::callback_functions.push_back([this](T){updatePreprocessorBlock();});
  updatePreprocessorBlock();
}


} // namespace renderer
} // namespace glrt
