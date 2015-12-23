#include <glrt/system.h>

namespace glrt {


QPixmap System::defaultSplashscreenPixmap()
{
  QPixmap pixmap(640, 320);

  pixmap.fill(QColor::fromRgb(0));

  return pixmap;
}


} // namespace glrt
