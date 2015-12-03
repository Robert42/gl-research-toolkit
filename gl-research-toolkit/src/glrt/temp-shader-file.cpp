#include <glrt/temp-shader-file.h>

#include <glhelperconfig.hpp>

namespace glrt {


TempShaderFile::TempShaderFile()
{
}

TempShaderFile::~TempShaderFile()
{
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

  tempShaderDir.makeAbsolute();
}

QDir TempShaderFile::tempDir()
{
  return QDir(GLRT_TEMP_SHADER_DIR);
}


} // namespace glrt
