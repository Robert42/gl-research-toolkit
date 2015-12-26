#ifndef GLRT_RENDERER_SPLASHSCREENSTYLE_H
#define GLRT_RENDERER_SPLASHSCREENSTYLE_H

#include <glrt/dependencies.h>
#include <QPainter>
#include <QSplashScreen>

namespace glrt {
namespace renderer {

class SplashscreenStyle
{
public:
  SplashscreenStyle();
  virtual ~SplashscreenStyle();

  SplashscreenStyle(const SplashscreenStyle&) = delete;
  SplashscreenStyle(SplashscreenStyle&&) = delete;
  SplashscreenStyle& operator=(const SplashscreenStyle&) = delete;
  SplashscreenStyle& operator=(SplashscreenStyle&&) = delete;

  QSplashScreen* createQSplashScreen(bool takeOwnershipOfStyle=false);

  static SplashscreenStyle* createFallbackStyle(const QString& title, const QSize& size=QSize(640, 240), int titleMargin=16, int messageSize=24, int messageMargin=4);

protected:
  virtual void paint(QPainter* painter, const QString& message) = 0;
  virtual QPixmap createPixmap() = 0;
};

} // namespace renderer
} // namespace glrt

#endif // GLRT_RENDERER_SPLASHSCREENSTYLE_H
