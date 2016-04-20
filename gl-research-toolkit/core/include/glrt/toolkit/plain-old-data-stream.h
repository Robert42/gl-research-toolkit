#ifndef GLRT_PLAINOLDDATASTREAM_H
#define GLRT_PLAINOLDDATASTREAM_H

#include <QIODevice>
#include <glrt/dependencies.h>

namespace glrt {

template<typename T>
void writeValue(QIODevice& device, const T& value);

template<typename T>
T readValue(QIODevice& device);

QIODevice& operator<<(QIODevice& device, quint8 value);
QIODevice& operator<<(QIODevice& device, quint16 value);
QIODevice& operator<<(QIODevice& device, quint32 value);
QIODevice& operator<<(QIODevice& device, quint64 value);

QIODevice& operator>>(QIODevice& device, quint8& value);
QIODevice& operator>>(QIODevice& device, quint16& value);
QIODevice& operator>>(QIODevice& device, quint32& value);
QIODevice& operator>>(QIODevice& device, quint64& value);

template<typename T>
void writeValue(QIODevice& device, const T& value)
{
  device.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
void writeValue(QByteArray& device, const T& value)
{
  device.append(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
T readValue(QIODevice& device)
{
  T value;
  if(device.read(reinterpret_cast<char*>(&value), sizeof(T)) != sizeof(T))
    throw GLRT_EXCEPTION("readValue<T> IO error");
  return value;
}

} // namespace glrt

#endif // GLRT_PLAINOLDDATASTREAM_H
