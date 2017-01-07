#include <glrt/scene/resources/texture-file.h>
#include <glrt/scene/resources/utilities/calculate_ibl_cubemaps.h>

namespace glrt {
namespace scene {
namespace resources {

TextureFile::IblCalculator::Implementation* TextureFile::IblCalculator::Implementation::_singleton = nullptr;

typedef TextureFile::GlTexture GlTexture;

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
    max_mipmap_level(0)
{
  if(type == Type::GGX)
  {
    max_mipmap_level = int(glm::floor(glm::log2<float>(size)));
    Q_ASSERT((uint(1) << uint(max_mipmap_level)) == glm::floorPowerOfTwo(uint(size)));
  }

  QVector<byte> rawData;

  for(int layer=0; layer<6; ++layer)
  {
    GlTexture& texture = target_textures[layer];

    for(int level=max_mipmap_level; level>=0; --level)
    {
      uint s = uint(size) >> uint(level);
      Q_ASSERT(s>0);

      GlTexture::UncompressedImage format = GlTexture::format(glm::uvec3(s, s, 1), level, GlTexture::Format::RGBA, GlTexture::Type::FLOAT16, GlTexture::Target::TEXTURE_2D);
      rawData.resize(glm::max<int>(int(format.rawDataLength), rawData.length()));
      texture.setUncompressed2DImage(format, rawData.data());
    }
    texture.makeComplete();
    GL_CALL(glTextureParameteri, texture.textureId, GL_TEXTURE_BASE_LEVEL, 0);
    GL_CALL(glTextureParameteri, texture.textureId, GL_TEXTURE_MAX_LEVEL, max_mipmap_level);
  }
}

void TextureFile::IblCalculator::execute(const TextureFile::GlTexture& source_texture)
{
  for(int layer=0; layer<6; layer++)
  {
    GlTexture::Target target = targetForLayer(layer);
    glm::mat4 rotation = rotationForLayer(layer);

    for(int level=0; level<=max_mipmap_level; ++level)
    {
      Implementation::singleton().execute(this, source_texture, target, layer, level, rotation);
    }
  }

  file->appendCubemapImageToTarget(this->target_textures, GlTexture::Type::FLOAT16, GlTexture::Format::RGB, max_mipmap_level);
}



void TextureFile::calculate_ibl_ggx_cubemap(const GlTexture& texture)
{
  IblCalculator calculator(this, IblCalculator::Type::GGX);

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


GlTexture::Target TextureFile::IblCalculator::targetForLayer(int layer)
{
  switch(layer)
  {
  case 0:
    return Target::CUBE_MAP_POSITIVE_X;
  case 1:
    return Target::CUBE_MAP_NEGATIVE_X;
  case 2:
    return Target::CUBE_MAP_POSITIVE_Y;
  case 3:
    return Target::CUBE_MAP_NEGATIVE_Y;
  case 4:
    return Target::CUBE_MAP_NEGATIVE_Z;
  case 5:
    return Target::CUBE_MAP_POSITIVE_Z;
  }

  return Target::CUBE_MAP_POSITIVE_X;
}

glm::mat4 TextureFile::IblCalculator::rotationForLayer(int layer)
{
  switch(targetForLayer(layer))
  {
  case Target::CUBE_MAP_POSITIVE_X:
  case Target::CUBE_MAP_NEGATIVE_X:
  case Target::CUBE_MAP_POSITIVE_Y:
  case Target::CUBE_MAP_NEGATIVE_Y:
  case Target::CUBE_MAP_POSITIVE_Z:
  case Target::CUBE_MAP_NEGATIVE_Z:
    return glm::mat4(1); // TODO::
  case Target::TEXTURE_1D:
  case Target::TEXTURE_2D:
  case Target::TEXTURE_3D:
    Q_UNREACHABLE();
  }
  Q_UNREACHABLE();
}

} // namespace resources
} // namespace scene
} // namespace glrt
