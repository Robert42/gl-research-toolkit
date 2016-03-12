#include <glrt/toolkit/plain-old-data-stream.h>

namespace glrt {


QIODevice& operator<<(QIODevice& device, quint8 value)
{
  writeValue(device, value);
  return device;
}

QIODevice& operator<<(QIODevice& device, quint16 value)
{
  writeValue(device, value);
  return device;
}

QIODevice& operator<<(QIODevice& device, quint32 value)
{
  writeValue(device, value);
  return device;
}

QIODevice& operator<<(QIODevice& device, quint64 value)
{
  writeValue(device, value);
  return device;
}

QIODevice& operator>>(QIODevice& device, quint8& value)
{
  value = readValue<quint8>(device);
  return device;
}

QIODevice& operator>>(QIODevice& device, quint16& value)
{
  value = readValue<quint16>(device);
  return device;
}

QIODevice& operator>>(QIODevice& device, quint32& value)
{
  value = readValue<quint32>(device);
  return device;
}

QIODevice& operator>>(QIODevice& device, quint64& value)
{
  value = readValue<quint64>(device);
  return device;
}


} // namespace glrt
