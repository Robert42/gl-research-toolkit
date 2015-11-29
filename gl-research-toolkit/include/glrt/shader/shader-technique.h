#ifndef GLRF_SHADER_TECHNIQUE_H
#define GLRF_SHADER_TECHNIQUE_H

#include <glrt/dependencies.h>

namespace glrf {
namespace shader {


class Technique : public QObject
{
  Q_OBJECT
public:
  class Manager;
  class Implementation;

  Technique(const Technique&) = delete;
  Technique(Technique&&) = delete;
  Technique& operator=(const Technique&) = delete;
  Technique& operator=(Technique&&) = delete;

  Technique(const QString& filename, Manager*manager);
  virtual ~Technique();

protected:
  virtual QVector<Implementation*> initImplementations() = 0;

private:
  QVector<Implementation*> implementations;
};


class Technique::Implementation : public QObject
{
  Q_OBJECT
public:
  Implementation(const Implementation&) = delete;
  Implementation(Implementation&&) = delete;
  Implementation& operator=(const Implementation&) = delete;
  Implementation& operator=(Implementation&&) = delete;

  Implementation(const QString& macroName, Technique* technique);
};


class Technique::Manager final : public QObject
{
  Q_OBJECT
public:
  Manager(const Manager&) = delete;
  Manager(Manager&&) = delete;
  Manager& operator=(const Manager&) = delete;
  Manager& operator=(Manager&&) = delete;

  Manager();
  ~Manager();

private:
  friend class Technique;

  QHash<QString, Technique*> allTechniques;

  void registerTechnique(const QString& name, Technique* technique);
};


} // namespace shader
} // namespace glrf

#endif // GLRF_SHADER_TECHNIQUE_H
