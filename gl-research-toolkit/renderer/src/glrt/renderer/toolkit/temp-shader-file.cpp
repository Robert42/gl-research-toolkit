#include <glrt/renderer/toolkit/temp-shader-file.h>

#include <glhelperconfig.hpp>

namespace glrt {
namespace renderer {


int TempShaderFile::instanceCounter = 0;


TempShaderFile::TempShaderFile()
{
  if(instanceCounter == 0)
    clearTempDir();

  instanceCounter++;
}

TempShaderFile::~TempShaderFile()
{
  instanceCounter--;

  if(instanceCounter == 0)
    clearTempDir();
}

void TempShaderFile::addPreprocessorBlock(const QStringList& preprocessorBlock)
{
  QString preprocessorBlockFile = tempDir().absoluteFilePath("preprocessor-block.h");

  QFile file(preprocessorBlockFile);

  if(!file.open(QFile::Append|QFile::WriteOnly|QFile::Text))
    throw GLRT_EXCEPTION(QString("Couldn't open the temp-shader file <%0> for writing.").arg(preprocessorBlockFile));

  QTextStream textStream(&file);

  textStream << preprocessorBlock.join('\n') << '\n';
}

void TempShaderFile::init()
{
  clearTempDir();

  gl::Details::ShaderIncludeDirManager::addIncludeDirs(tempDir());
}

void TempShaderFile::clearTempDir()
{
  QDir tempShaderDir = tempDir();

  if(tempShaderDir.exists())
    if(!tempShaderDir.removeRecursively())
      throw GLRT_EXCEPTION(QString("Couldn't remove the directory %0").arg(tempShaderDir.absolutePath()));

  QDir parentDir = tempShaderDir;
  parentDir.cdUp();
  parentDir.mkdir(tempShaderDir.dirName());
}

QDir TempShaderFile::tempDir()
{
  return QDir(GLRT_TEMP_SHADER_DIR);
}


} // namespace renderer
} // namespace glrt
