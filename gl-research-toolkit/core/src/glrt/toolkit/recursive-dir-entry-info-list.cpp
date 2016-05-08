#include <glrt/toolkit/recursive-dir-entry-info-list.h>

namespace glrt {

void recursiveDirEntryInfoList(const QDir& directory,
                               QFileInfoList& directories,
                               QFileInfoList& filteredFiles,
                               QDir::Filter filter,
                               bool followSymlinks,
                               int maxRecursiveDepth)
{
  if(!directory.exists())
    return;

  filteredFiles += directory.entryInfoList(filter, QDir::Name);

  if(maxRecursiveDepth == 0)
    return;

  QDir::Filters dirFilter = QDir::AllDirs | QDir::NoDotAndDotDot;

  if(!followSymlinks)
    dirFilter |= QDir::NoSymLinks;

  QFileInfoList subDirs = directory.entryInfoList(dirFilter, QDir::Name);

  directories += subDirs;

  for(const QFileInfo& subDir : subDirs)
  {
    if(!subDir.isDir())
      continue;

    recursiveDirEntryInfoList(QDir(subDir.absoluteFilePath()),
                              directories,
                              filteredFiles,
                              filter,
                              followSymlinks,
                              maxRecursiveDepth-1);
  }
}

} // namespace glrt
