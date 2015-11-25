#include <glrt/shader/shader-factory.h>

/*! \fn gl::ShaderObject* ShaderObject::create()

\brief Creates a new ShaderObject

\note The caller owns the turned object.

*/

namespace glrt {
namespace shader {


Factory::Factory()
{
}


Factory::~Factory()
{
}


// ======== CustomFactory ======================================================


CustomFactory::CustomFactory(const QFileInfo& vertexShader,
                             const QFileInfo& fragmentShader)
{
  shaderFiles.insert(gl::ShaderObject::ShaderType::VERTEX, vertexShader);
  shaderFiles.insert(gl::ShaderObject::ShaderType::FRAGMENT, fragmentShader);
}


gl::ShaderObject* CustomFactory::create(const QString& name) const
{
  gl::ShaderObject* shader = new gl::ShaderObject(name.toStdString());

  Q_ASSERT(!shaderFiles.isEmpty());

  for(gl::ShaderObject::ShaderType shaderType : shaderFiles.keys())
    shader->AddShaderFromFile(shaderType, shaderFiles[shaderType].absoluteFilePath().toStdString());
  shader->CreateProgram();

  return shader;
}


// ======== TechniqueBasedFactory ==============================================


TechniqueBasedFactory::TechniqueBasedFactory(const QVector<Technique*>& techniques)
  : techniques(techniques)
{
}


gl::ShaderObject* TechniqueBasedFactory::create(const QString& name) const
{
  Q_UNUSED(name);
  return nullptr; // FIXME: returning nullptr is not allowed
}


} // namespace shader
} // namespace glrt

