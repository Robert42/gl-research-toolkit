#ifndef GLRT_RECURSIVEDIRENTRYINFOLIST_H
#define GLRT_RECURSIVEDIRENTRYINFOLIST_H

#include <QDir>

namespace glrt {

void recursiveDirEntryInfoList(const QDir& directory,
                               QFileInfoList& directories,
                               QFileInfoList& filteredFiles,
                               QDir::Filter filter,
                               bool followSymlinks=true,
                               int maxRecursiveDepth = 1024);

} // namespace glrt

#endif // GLRT_RECURSIVEDIRENTRYINFOLIST_H
