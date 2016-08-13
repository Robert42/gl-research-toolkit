#include <glrt/renderer/transformation-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>

namespace glrt {
namespace renderer {

TransformationBuffer::TransformationBuffer(quint16 capacity)
  : alignment(aligned_vector<UniformData>::retrieveAlignmentOffset(aligned_vector<UniformData>::Alignment::UniformBufferOffsetAlignment)),
    buffer(static_cast<GLsizeiptr>(alignment) * static_cast<GLsizeiptr>(capacity), gl::Buffer::MAP_WRITE)
{
}

TransformationBuffer::~TransformationBuffer()
{
}


TransformationBuffer::TransformationBuffer(TransformationBuffer&& other)
  : alignment(other.alignment),
    buffer(std::move(other.buffer))
{
}

TransformationBuffer& TransformationBuffer::operator=(TransformationBuffer&& other)
{
  this->alignment = std::move(other.alignment);
  this->buffer = std::move(other.buffer);
  return *this;
}

void TransformationBuffer::update(quint16 begin, quint16 end, const scene::Scene::Data::Transformations& transformations)
{
  const quint16 num_elemnts_to_copy = end - begin;

  quint8* tempBuffer = reinterpret_cast<quint8*>(buffer.Map(static_cast<GLintptr>(begin) * static_cast<GLsizeiptr>(sizeof(glm::mat4)), static_cast<GLsizeiptr>(num_elemnts_to_copy) * static_cast<GLsizeiptr>(alignment), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));

#pragma omp simd
  for(quint16 i=0; i<num_elemnts_to_copy; ++i)
  {
    UniformData& data = *reinterpret_cast<UniformData*>(tempBuffer + size_t(i) * size_t(alignment));
    data.transformation = transformations.globalCoordFrame(i).toMat4();
  }

  buffer.Unmap();
}

GLuint64 TransformationBuffer::gpuAddressForInstance(quint16 i) const
{
  Q_ASSERT(GLuint64(i+1) * GLuint64(alignment) <= GLuint64(this->buffer.GetSize()));

  return this->buffer.gpuBufferAddress() + GLuint64(i) * GLuint64(alignment);
}


} // namespace renderer
} // namespace glrt
