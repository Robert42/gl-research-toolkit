#include <glrt/renderer/transformation-buffer.h>
#include <glrt/renderer/toolkit/aligned-vector.h>

namespace glrt {
namespace renderer {

TransformationBuffer::TransformationBuffer()
{
  alignment = aligned_vector<UniformData>::retrieveAlignmentOffset(aligned_vector<UniformData>::Alignment::UniformBufferOffsetAlignment);
}

TransformationBuffer::~TransformationBuffer()
{
}


TransformationBuffer::TransformationBuffer(TransformationBuffer&& other)
  : buffer(std::move(other.buffer)),
    alignment(other.alignment)
{
}

TransformationBuffer& TransformationBuffer::operator=(TransformationBuffer&& other)
{
  this->buffer = std::move(other.buffer);
  this->alignment = std::move(other.alignment);
  return *this;
}

void TransformationBuffer::init(const glrt::scene::StaticMeshComponent** components, int length)
{
  this->buffer = std::move(gl::Buffer(alignment * length, gl::Buffer::MAP_WRITE));

  update(0, length, components, length);
}

void TransformationBuffer::update(int begin, int end, const glrt::scene::StaticMeshComponent** components, int length)
{
  const int num_elemnts_to_copy = end - begin;

  Q_ASSERT(buffer.GetSize() > (end-1)*alignment);
  Q_ASSERT(end<=length);
  Q_ASSERT(begin<=end);
  Q_ASSERT(end<=length);

  quint8* tempBuffer = reinterpret_cast<quint8*>(buffer.Map(begin * sizeof(glm::mat4), num_elemnts_to_copy * sizeof(glm::mat4), gl::Buffer::MapType::WRITE, gl::Buffer::MapWriteFlag::INVALIDATE_RANGE));

  for(int i=0; i<num_elemnts_to_copy; ++i)
  {
    UniformData& data = *reinterpret_cast<UniformData*>(tempBuffer + i * alignment);
    data.transformation = components[i]->globalCoordFrame().toMat4();
  }

  buffer.Unmap();
}

GLuint64 TransformationBuffer::gpuAddressForInstance(int i) const
{
  Q_ASSERT(GLuint64(i+1) * GLuint64(alignment) <= this->buffer.GetSize());

  return this->buffer.gpuBufferAddress() + GLuint64(i) * GLuint64(alignment);
}


} // namespace renderer
} // namespace glrt
