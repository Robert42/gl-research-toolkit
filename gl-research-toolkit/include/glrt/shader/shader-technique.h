#ifndef GLRF_SHADER_TECHNIQUE_H
#define GLRF_SHADER_TECHNIQUE_H

#include <glrt/dependencies.h>

namespace glrf {
namespace shader {

class Technique
{
public:
  typedef std::shared_ptr<Technique> Ptr;

  Technique(const Technique&) = delete;
  Technique(Technique&&) = delete;
  Technique& operator=(const Technique&) = delete;
  Technique& operator=(Technique&&) = delete;

  Technique();
  virtual ~Technique();
};

} // namespace shader
} // namespace glrf

#endif // GLRF_SHADER_TECHNIQUE_H
