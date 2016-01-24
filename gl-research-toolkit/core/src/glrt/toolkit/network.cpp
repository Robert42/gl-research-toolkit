#include <glrt/toolkit/network.h>

namespace glrt {
namespace Network {


bool readAtomic(QByteArray* byteArray,
                QIODevice& device,
                int msecWait,
                int msecWaitLoopIteration)
{
  bool waitForReadyRead = true;
  if(msecWait <= 0 || msecWaitLoopIteration <= 0)
  {
    msecWait = 1;
    msecWaitLoopIteration = 1;
    waitForReadyRead = false;
  }

  for(int t=0; t<msecWait; t+=msecWaitLoopIteration)
  {
    if(waitForReadyRead && !device.waitForReadyRead(msecWaitLoopIteration))
      continue;


    quintptr size;
    if(device.peek(reinterpret_cast<char*>(&size), sizeof(quintptr)) == sizeof(quintptr))
    {
       if(static_cast<quintptr>(device.bytesAvailable()) >= sizeof(quintptr)+size)
       {
         quintptr size_redundant;
         if(device.read(reinterpret_cast<char*>(&size_redundant), sizeof(quintptr)) != sizeof(quintptr))
           throw GLRT_EXCEPTION("readAtomic inconsistent size reading behavior");
         if(size_redundant != size)
           throw GLRT_EXCEPTION("readAtomic inconsistent block size");
         *byteArray = device.read(size);
         return true;
       }
    }
  }

  return false;
}

void writeAtomic(QIODevice *device,
                 const QByteArray& data)
{
  quintptr size = data.size();
  device->write(reinterpret_cast<char*>(&size), sizeof(size));
  device->write(data);
}


} // namespace Network
} // namespace glrt

