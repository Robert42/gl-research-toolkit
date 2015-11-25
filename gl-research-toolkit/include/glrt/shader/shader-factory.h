#ifndef GLRT_SHADER_FACTORY_H
#define GLRT_SHADER_FACTORY_H

#include "shader-technique.h"

#include <glhelper/shaderobject.hpp>

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

  virtual gl::ShaderObject* create() const = 0;
};


class CustomFactory final : public Factory
{
public:

};


class TechniqueBasedFactory : public Factory
{
public:
  const QVector<Technique*> techniques;

  TechniqueBasedFactory(const QVector<Technique*>& techniques);

  gl::ShaderObject* create() const final override;
};


} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_FACTORY_H
