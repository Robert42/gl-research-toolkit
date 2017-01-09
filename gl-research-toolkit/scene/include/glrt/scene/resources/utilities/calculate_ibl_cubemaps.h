#include <glrt/scene/resources/texture-file.h>

namespace glrt {
namespace scene {
namespace resources {

class TextureFile::IblCalculator final
{
public:
  Q_DISABLE_COPY(IblCalculator)

  class Implementation;

  enum class Type
  {
    GGX,
    DIFFUSE,
    CONE_60,
    CONE_45,
  };

  IblCalculator(TextureFile* file, Type type, int size=512);

  TextureFile::GlTexture target_textures[6];
  Type type;
  int size;

  void execute(const TextureFile::GlTexture& source_texture);

  static Target targetForLayer(int layer);
  static glm::mat4 rotationForLayer(int layer);

private:
  TextureFile* file;

  int max_mipmap_level;
};

class TextureFile::IblCalculator::Implementation
{
  Q_DISABLE_COPY(Implementation)

public:
  Implementation();
  virtual ~Implementation();

  static Implementation& singleton();

  virtual void execute(IblCalculator* calculator, const TextureFile::GlTexture& texture, GlTexture::Target target_texture, int layer, int level, const glm::mat4& side_rotation) = 0;

private:
  static Implementation* _singleton;
};

} // namespace resources
} // namespace scene
} // namespace glrt
