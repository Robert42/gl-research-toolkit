#ifndef GLRT_RENDERER_ALIGNEDVECTOR_H
#define GLRT_RENDERER_ALIGNEDVECTOR_H

#include <glrt/renderer/dependencies.h>

namespace glrt {
namespace renderer {


template<typename T>
class aligned_vector
{
public:
  enum class Alignment
  {
    UniformBufferOffsetAlignment
  };

  aligned_vector(Alignment alignment)
  {
    switch(alignment)
    {
    case Alignment::UniformBufferOffsetAlignment:
      glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &_alignment);
      break;
    default:
      Q_UNREACHABLE();
    }
    _alignment = _alignment * ((_alignment-1+sizeof(T)) / _alignment);
  }

  void writeTo(void* target, size_t nElements=std::numeric_limits<size_t>::max()) const
  {
    nElements = glm::min<size_t>(nElements, size());

    memcpy(target, data(), nElements * _alignment);
  }

  void* data()
  {
    return &operator[](0);
  }

  const void* data() const
  {
    return &operator[](0);
  }

  GLint alignment() const
  {
    return _alignment;
  }

  void resize(size_t nElements)
  {
    internal_vector.resize(nElements*_alignment);
  }

  void reserve(size_t nElements)
  {
    internal_vector.reserve(nElements*_alignment);
  }

  size_t size() const
  {
    return internal_vector.size() / _alignment;
  }

  size_t size_in_bytes() const
  {
    return internal_vector.size();
  }

  void push_back(const T& value)
  {
    resize(size()+1);
    operator[](size()-1) = value;
  }

  T& operator[](size_t i)
  {
    return reinterpret_cast<T&>(internal_vector[i*_alignment]);
  }

  const T& operator[](size_t i) const
  {
    return reinterpret_cast<const T&>(internal_vector[i*_alignment]);
  }

private:
  GLint _alignment;
  std::vector<quint8> internal_vector;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_ALIGNEDVECTOR_H
