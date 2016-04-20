#include <glrt/scene/resources/voxel-file.h>
#include <glrt/toolkit/plain-old-data-stream.h>

namespace glrt {
namespace scene {
namespace resources {


VoxelFile::VoxelFile()
{
}

VoxelFile::~VoxelFile()
{
}


quint64 VoxelFile::magicNumber()
{
  return magicNumberForString("glrtvoxl");
}


void VoxelFile::load(const QFileInfo& fileInfo, const Uuid<StaticMesh>& meshUuid)
{
  Q_ASSERT(!meshUuid.toQUuid().isNull());

  QFile file(fileInfo.absoluteFilePath());

  if(file.size() <= qint64(sizeof(Header)))
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):Invalid file (too small) 0x1").arg(file.fileName()));

  if(!file.open(QFile::ReadOnly))
    throw GLRT_EXCEPTION(QString("Can't read voxel file <%0>").arg(fileInfo.absoluteFilePath()));


  Header header = readValue<Header>(file);

  if(header.magicNumber != VoxelFile::magicNumber())
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Wrong magic number %1.").arg(file.fileName()).arg(header.magicNumber));
  if(header.headerLength != sizeof(Header))
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unexpected Header size %1.").arg(file.fileName()).arg(header.headerLength));
  if(header.metaDataLength != sizeof(MetaData))
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unexpected VoxelFileMetaData length %1.").arg(file.fileName()).arg(header.metaDataLength));
  if(header.numVoxelFiles == 0)
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unexpected numVoxelFiles %1.").arg(file.fileName()).arg(header.numVoxelFiles));
  if(header._padding != 0)
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unexpected value in reserved padding bytes.").arg(file.fileName()).arg(header.numVoxelFiles));
  if(header.meshValidationUuid != 0)
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unexpected value in reserved padding bytes.").arg(file.fileName()).arg(header.numVoxelFiles));
  if(header.meshValidationUuid != meshUuid.toQUuid())
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Wrong mesh uuid: %1 expected: %2").arg(file.fileName()).arg(header.meshValidationUuid.toString()).arg(meshUuid.toString()));

  if(file.size() <= qint64(sizeof(Header) + sizeof(MetaData) * header.numVoxelFiles))
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):Invalid file (too small) 0x2").arg(file.fileName()));

  QVector<MetaData> metaData;
  metaData.resize(header.numVoxelFiles);

  if(file.read(reinterpret_cast<char*>(metaData.data()), sizeof(MetaData) * header.numVoxelFiles) != sizeof(MetaData) * header.numVoxelFiles)
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): Unknown IO error 0x1").arg(file.fileName()));

  QStringList files = QString::fromUtf8(file.readAll()).split("\n");

  if(files.length() != metaData.length())
    throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):Mismatch betwen the number of filenames and the metadata").arg(file.fileName()));

  this->meshUuid = meshUuid;
  this->textureFiles.clear();
  QDir dir(fileInfo.path());

  for(int i=0; i<metaData.length(); ++i)
  {
    if(metaData[i]._padding[0] != 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0100").arg(file.fileName()));
    if(metaData[i]._padding[1] != 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0101").arg(file.fileName()));
    if(metaData[i].factor != 1.f)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0102").arg(file.fileName()));
    if(metaData[i].offset != 0.f)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0103").arg(file.fileName()));
    if(glm::any(glm::isnan(metaData[i].localToVoxelSpace.position)))
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0104").arg(file.fileName()));
    if(glm::any(glm::isnan(glm::vec4(metaData[i].localToVoxelSpace.orientation.x,
                                     metaData[i].localToVoxelSpace.orientation.y,
                                     metaData[i].localToVoxelSpace.orientation.z,
                                     metaData[i].localToVoxelSpace.orientation.w))))
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0105").arg(file.fileName()));
    if(glm::isnan(metaData[i].localToVoxelSpace.scaleFactor))
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0106").arg(file.fileName()));
    if(metaData[i]._padding2 != 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0107").arg(file.fileName()));
    if(metaData[i].gridSize[0] <= 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0108").arg(file.fileName()));
    if(metaData[i].gridSize[1] <= 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x0109").arg(file.fileName()));
    if(metaData[i].gridSize[2] <= 0)
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0):0x010a").arg(file.fileName()));

    QFileInfo textureFile = dir.absoluteFilePath(files[i]);

    if(!textureFile.exists())
      throw GLRT_EXCEPTION(QString("VoxelFile::loadFromFile(%0): The taret texture file %1 doesn't exist").arg(file.fileName()).arg(textureFile.filePath()));

    this->textureFiles.insert(textureFile.absoluteFilePath(), metaData[i]);
  }
}


void VoxelFile::save(const QFileInfo& fileInfo)
{
  if(fileInfo.exists())
    QFile::remove(fileInfo.absoluteFilePath());

  if(textureFiles.isEmpty() || meshUuid.toQUuid().isNull())
    return;

  Header header;

  QByteArray data;

  Q_ASSERT(textureFiles.size() < std::numeric_limits<quint16>::max());
  header.numVoxelFiles = quint16(textureFiles.size());

  data.append(reinterpret_cast<char*>(&header), sizeof(Header));
  for(const MetaData& metaData : textureFiles.values())
    data.append(reinterpret_cast<const char*>(&metaData), sizeof(MetaData));
  QStringList relativeFilePaths;
  for(const QFileInfo& textureFile : textureFiles.keys())
  {
    QString relativeFilePath = QDir(fileInfo.path()).relativeFilePath(textureFile.absoluteFilePath());
    relativeFilePaths.append(relativeFilePath);
  }
  data.append(relativeFilePaths.join("\n").toUtf8());

  QFile file(fileInfo.absoluteFilePath());
  if(!file.open(QFile::WriteOnly))
    throw GLRT_EXCEPTION(QString("Can't write voxel metadata file %0").arg(fileInfo.absoluteFilePath()));
  file.write(data);
}


} // namespace resources
} // namespace scene
} // namespace glrt
