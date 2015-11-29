#ifndef GLRT_SHADER_SHADER_H
#define GLRT_SHADER_SHADER_H

#include <glrt/dependencies.h>

#include "shader-technique.h"

namespace glrt {
namespace shader {


class Shader final : public QObject
{
  Q_OBJECT
public:
  class Manager;

  Shader(const Shader&) = delete;
  Shader(Shader&&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader& operator=(Shader&&) = delete;

private:
  Shader(const QString& filename);
};


class Shader::Manager final : public QObject
{
  Q_OBJECT
public:
  Manager(const Manager&) = delete;
  Manager(Manager&&) = delete;
  Manager& operator=(const Manager&) = delete;
  Manager& operator=(Manager&&) = delete;

  Manager();
};


} // namespace shader
} // namespace glrt

#endif // GLRT_SHADER_SHADER_H
