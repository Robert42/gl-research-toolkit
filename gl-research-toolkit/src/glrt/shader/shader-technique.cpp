#include <glrt/shader/shader-technique.h>

namespace glrf {
namespace shader {


// ======== Technique ==========================================================


Technique::Technique(const QString& filename, Manager* manager)
  : QObject(manager)
{
  manager->registerTechnique(filename, this);
}

Technique::~Technique()
{
}


// ======== Technique::Implementation ==========================================


Technique::Implementation::Implementation(const QString& macroName, Technique* technique)
  : QObject(technique)
{
}


// ======== Technique::Manager =================================================


Technique::Manager::Manager()
{
}

Technique::Manager::~Manager()
{
}



} // namespace shader
} // namespace glrf

