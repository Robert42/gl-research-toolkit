#ifndef GLRT_SHADER_FACTORY_H
#define GLRT_SHADER_FACTORY_H

#include "shader-technique.h"

namespace glrt {
namespace shader {

class Technique;
class Factory
{
public:
  Factory(const Factory&) = delete;
  Factory(Factory&&) = delete;
  Factory& operator=(const Factory&) = delete;
  Factory& operator=(Factory&&) = delete;

  Factory();
  virtual ~Factory();

  virtual gl::ShaderObject* create(const QString& name) const = 0;
};


class CustomFactory final : public Factory
{
public:
  QHash<gl::ShaderObject::ShaderType, QFileInfo> shaderFiles;

  CustomFactory(const QFileInfo& vertexShader,
                const QFileInfo& fragmentShader);

  gl::ShaderObject* create(const QString& name) const final override;
};


class TechniqueBasedFactory : public Factory
{
public:
  const QVector<Technique*> techniques;

  TechniqueBasedFactory(const QVector<Technique*>& techniques);

  gl::ShaderObject* create(const QString& name) const final override;
};


} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_FACTORY_H
