#include <glrt/scene/resources/texture-file.h>
#include <glrt/scene/resources/utilities/calculate_ibl_cubemaps.h>

namespace glrt {
namespace scene {
namespace resources {

TextureFile::IblCalculator::Implementation* TextureFile::IblCalculator::Implementation::_singleton = nullptr;

TextureFile::IblCalculator::Implementation::Implementation()
{
  if(_singleton == nullptr)
    _singleton = this;
}

TextureFile::IblCalculator::Implementation::~Implementation()
{
  if(_singleton == this)
    _singleton = nullptr;
}

TextureFile::IblCalculator::Implementation& TextureFile::IblCalculator::Implementation::singleton()
{
  Q_ASSERT(_singleton != nullptr);
  return *_singleton;
}


TextureFile::IblCalculator::IblCalculator(TextureFile* file, Type type, int size)
  : type(type),
    size(size),
    file(file),
    max_mipmap_layer(0)
{
  if(type == Type::GGX)
    max_mipmap_layer = int(glm::floor(glm::log2<float>(size)));
}

void TextureFile::IblCalculator::execute(const TextureFile::GlTexture& source_texture)
{
  GlTexture::Target target = Target::CUBE_MAP_NEGATIVE_Z;
  glm::mat4 rotation = glm::mat4(1);
  int layer = 0;

  for(int i=0; i<6; i++)
  {
    switch(i)
    {
    case 0:
      target = Target::CUBE_MAP_NEGATIVE_X;
      rotation = glm::mat4(1); // TODO::
      break;
    case 1:
      target = Target::CUBE_MAP_POSITIVE_X;
      rotation = glm::mat4(1); // TODO::
      break;
    case 2:
      target = Target::CUBE_MAP_NEGATIVE_Y;
      rotation = glm::mat4(1); // TODO::
      break;
    case 3:
      target = Target::CUBE_MAP_POSITIVE_Y;
      rotation = glm::mat4(1); // TODO::
      break;
    case 4:
      target = Target::CUBE_MAP_NEGATIVE_Z;
      rotation = glm::mat4(1); // TODO::
      break;
    case 5:
      target = Target::CUBE_MAP_POSITIVE_Z;
      rotation = glm::mat4(1); // TODO::
      break;
    }
    layer =  i;// TODO::

    for(int level=0; i<=max_mipmap_layer; ++i)
    {
      Implementation::singleton().execute(this, source_texture, target, layer, level, rotation);

      file->appendImageToTarget(target, this->target_texture, GlTexture::Type::FLOAT16, GlTexture::Format::RGB, level);
    }
  }
}



void TextureFile::calculate_ibl_ggx_cubemap(const GlTexture& texture)
{
  IblCalculator calculator(this, IblCalculator::Type::GGX, true);

  calculator.execute(texture);
}

void TextureFile::calculate_ibl_diffuse_cubemap(const GlTexture& texture)
{
  IblCalculator calculator(this, IblCalculator::Type::DIFFUSE);

  calculator.execute(texture);
}

void TextureFile::calculate_ibl_cone_60_cubemap(const GlTexture& texture)
{
  IblCalculator calculator(this, IblCalculator::Type::CONE_60);

  calculator.execute(texture);
}

void TextureFile::calculate_ibl_cone_45(const GlTexture& texture)
{
  IblCalculator calculator(this, IblCalculator::Type::CONE_45);

  calculator.execute(texture);
}

} // namespace resources
} // namespace scene
} // namespace glrt
