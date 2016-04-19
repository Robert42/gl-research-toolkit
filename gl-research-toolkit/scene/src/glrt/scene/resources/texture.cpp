#include <glrt/scene/resources/texture.h>
#include <glrt/scene/resources/texture-file.h>

namespace glrt {
namespace scene {
namespace resources {

class Texture::Source
{
public:
  virtual ~Source();

  virtual bool isEmpty() const = 0;
  virtual GLuint load() = 0;
  virtual QDebug print(QDebug d) = 0;
};

Texture::Source::~Source()
{
}

// ====

class Texture::FileSource final : public Source
{
public:
  QFileInfo file;

  FileSource(QFileInfo file);

  bool isEmpty() const final override;

  GLuint load() final override;

  QDebug print(QDebug d) final override;
};

Texture::FileSource::FileSource(QFileInfo file)
  : file(file)
{
}

bool Texture::FileSource::isEmpty() const
{
  return file.filePath().isEmpty();
}

GLuint Texture::FileSource::load()
{
  return glrt::scene::resources::TextureFile::loadFromFile(this->file);
}

QDebug Texture::FileSource::print(QDebug d)
{
  return d << "Texture("<<file.absoluteFilePath()<<")";
}

// ====

class Texture::Empty final : public Source
{
public:
  QFileInfo file;

  Empty();

  bool isEmpty() const final override;

  GLuint load() final override;

  QDebug print(QDebug d) final override;
};

Texture::Empty::Empty()
{
}

bool Texture::Empty::isEmpty() const
{
  return true;
}

GLuint Texture::Empty::load()
{
  Q_ASSERT_X(false, "Texture::Empty::load()", "Trying to load from an empty source");
  return 0;
}

QDebug Texture::Empty::print(QDebug d)
{
  return d << "Texture()";
}

// ====

Texture::Texture()
{
}

bool Texture::isEmpty() const
{
  return source->isEmpty();
}

QDebug operator<<(QDebug d, const Texture& t)
{
  return t.source->print(d);
}

GLuint Texture::load()
{
  return source->load();;
}

void Texture::clear()
{
  source = QSharedPointer<Source>(new Empty);
}

void Texture::setFile(const QFileInfo& file)
{
  source = QSharedPointer<Source>(new FileSource(file));
}

} // namespace resources
} // namespace scene
} // namespace glrt
