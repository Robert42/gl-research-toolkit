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


// ======== TechniqueBasedFactory ==============================================


TechniqueBasedFactory::TechniqueBasedFactory(const QVector<Technique*>& techniques)
  : techniques(techniques)
{
}


gl::ShaderObject* TechniqueBasedFactory::create() const
{
  return nullptr; // FIXME: returning nullptr is not allowed
}


} // namespace shader
} // namespace glrt

