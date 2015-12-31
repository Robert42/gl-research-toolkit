#ifndef GLRT_ASSIMPGLMCONVERTER_H
#define GLRT_ASSIMPGLMCONVERTER_H

#include <glrt/dependencies.h>
#include <assimp/types.h>


namespace glrt {


inline glm::vec3 to_glm_vec3(const aiVector3D& v)
{
  return reinterpret_cast<const glm::vec3&>(v);
}

inline glm::vec2 to_glm_vec2(const aiVector2D& v)
{
  return reinterpret_cast<const glm::vec2&>(v);
}

inline glm::mat4 to_glm_mat4(const aiMatrix4x4& m)
{
  return glm::transpose(reinterpret_cast<const glm::mat4&>(m));
}


} // namespace glrt


#endif // GLRT_ASSIMPGLMCONVERTER_H

