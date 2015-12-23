#include <glrt/system.h>

namespace glrt {


QPixmap System::defaultSplashscreenPixmap(const QString& title)
{
  QPixmap pixmap(640, 320);

  // #TODO: draw the title

  pixmap.fill(QColor::fromRgb(0));

  return pixmap;
}


} // namespace glrt
