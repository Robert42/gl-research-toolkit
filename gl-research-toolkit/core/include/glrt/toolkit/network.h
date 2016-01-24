#ifndef GLRT_NETWORK_NETWORK_H
#define GLRT_NETWORK_NETWORK_H

#include <glrt/dependencies.h>

#include <QBuffer>

namespace glrt {
namespace Network {

bool readAtomic(QByteArray* byteArray,
                QIODevice& device,
                int msecWait,
                int msecWaitLoopIteration=10);
void writeAtomic(QIODevice* device,
                 const QByteArray& data);

} // namespace Network
} // namespace glrt

#endif // GLRT_NETWORK_NETWORK_H
