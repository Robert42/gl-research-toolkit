#ifndef GLRT_SCENE_RESOURCES_TEXTUREFILE_H
#define GLRT_SCENE_RESOURCES_TEXTUREFILE_H

#include <glrt/dependencies.h>
#include <glrt/toolkit/array.h>

#include "utilities/gl-texture.h"

#include <QFileInfo>

namespace glrt {
namespace scene {
namespace resources {

class TextureFile : public QObject
{
  Q_OBJECT
public:
  class IblCalculator;

  typedef utilities::GlTexture GlTexture;
  typedef GlTexture::Target Target;
  typedef GlTexture::Type Type;
  typedef GlTexture::Format Format;
  typedef GlTexture::Compression Compression;
  typedef GlTexture::TextureAsFloats TextureAsFloats;
  typedef GlTexture::UncompressedImage UncompressedImage;
  typedef GlTexture::CompressedImage CompressedImage;

  struct ImportSettings : public GlTexture::ImportSettings
  {
    Target target = Target::TEXTURE_2D;
    Type type = Type::UINT8;
    Format format = Format::RGBA;
    Compression compression = Compression::NONE;
    bool calculate_dfg_lut = false;
    bool calculate_ibl_ggx_cubemap = false;
    bool calculate_ibl_diffuse_cubemap = false;
    bool calculate_ibl_cone_60 = false;
    bool calculate_ibl_cone_45 = false;
    bool remapSourceAsSigned = false;
    glm::vec4 offset = glm::vec4(0);
    glm::vec4 factor = glm::vec4(1);
    std::string red_channel_suffix;
    std::string green_channel_suffix;
    std::string blue_channel_suffix;
    std::string alpha_channel_suffix;
    bool merge_red_as_grey = true;
    bool merge_green_as_grey = true;
    bool merge_blue_as_grey = true;
    bool merge_alpha_as_grey = true;
    glm::vec4 force_value = glm::vec4(NAN);

    void preprocess()
    {
      if(!glm::isnan(force_value.r))
      {
        offset.r = force_value.r;
        factor.r = 0;
      }
      if(!glm::isnan(force_value.g))
      {
        offset.g = force_value.g;
        factor.g = 0;
      }
      if(!glm::isnan(force_value.b))
      {
        offset.b = force_value.b;
        factor.b = 0;
      }
      if(!glm::isnan(force_value.a))
      {
        offset.a = force_value.a;
        factor.a = 0;
      }
    }

    bool need_remapping() const
    {
      return factor!=glm::vec4(1) || offset!=glm::vec4(0) || force_value!=glm::vec4(NAN);
    }

    bool need_merging() const
    {
      return !red_channel_suffix.empty() || !green_channel_suffix.empty() || !blue_channel_suffix.empty() || !alpha_channel_suffix.empty();
    }

    bool need_processing() const
    {
      return remapSourceAsSigned || remapSourceAsSigned || need_remapping() || need_merging() || calculate_dfg_lut || result_is_cubemap();
    }

    bool result_is_cubemap() const
    {
      return calculate_ibl_ggx_cubemap || calculate_ibl_diffuse_cubemap || calculate_ibl_cone_60 || calculate_ibl_cone_45;
    }

    static void registerType();
  };

  TextureFile();

  void appendUncompressedImage(UncompressedImage image, const QVector<byte>& rawData);
  void appendImage(const GlTexture& texture, Type type = Type::UINT8, Format format = Format::RGBA, int mipmapLevel=0);
  void appendCubemapImageToTarget(GlTexture* side_textures, Type type, Format format, int max_mipmap_level);

  void import(const QFileInfo& srcFile, ImportSettings importSettings);
  void save(const QFileInfo& textureFile);
  static GLuint loadFromFile(const QFileInfo& textureFile);

private:

  struct Header
  {
    quint64 magicNumber = TextureFile::magicNumber();
    quint16 headerLength = sizeof(Header);
    quint16 uncompressedImageSize = sizeof(UncompressedImage);
    quint16 compressedImageSize = 0;
    quint16 numUncompressedImages = 0;
    quint16 numCompressedImages = 0;
    padding<quint16, 3> _padding;
  };

  QVector<CompressedImage> compressedImages;
  QVector<UncompressedImage> uncompressedImages;
  QVector<QVector<byte>> rawData;


  quint32 appendRawData(const QVector<byte>& rawData);
  int expectedFileSize() const;

  void clear();

  static quint64 magicNumber();

  void calculate_ibl_ggx_cubemap(const GlTexture& texture);
  void calculate_ibl_diffuse_cubemap(const GlTexture& texture);
  void calculate_ibl_cone_60_cubemap(const GlTexture& texture);
  void calculate_ibl_cone_45(const GlTexture& texture);
};

} // namespace resources
} // namespace scene
} // namespace glrt

#endif // GLRT_SCENE_RESOURCES_TEXTUREFILE_H
