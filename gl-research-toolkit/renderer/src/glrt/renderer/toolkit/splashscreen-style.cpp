#include <glrt/renderer/toolkit/splashscreen-style.h>
#include <glrt/renderer/toolkit/logger.h>

#include <QApplication>
#include <QThread>
#include <QPainter>

namespace glrt {
namespace renderer {


SplashscreenStyle::SplashscreenStyle()
{
}

SplashscreenStyle::~SplashscreenStyle()
{
}

typedef std::function<void(const QString&)> MessageHandler;

inline MessageHandler& getSplashscreenMessageHandler()
{
  static MessageHandler messageHandler;
  return messageHandler;
}


void showSplashscreenMessage(const QString& message)
{
  MessageHandler& messageHandler = getSplashscreenMessageHandler();

  if(messageHandler)
    messageHandler(message);

  Logger::SuppressDebug suppressLog;
  qDebug() << "Show SplashScreen message: " << message;
  Q_UNUSED(suppressLog);
}


/*!
\brief Creates a new instance of QSplashScreen

\param If true, the returned splashscreen will take ownership of the called
       SplashscreenStyle instance.

\return

\note The callers takes the ownership of the returned instance.
 */
QSplashScreen* SplashscreenStyle::createQSplashScreen(bool takeOwnershipOfStyle)
{
  class CustomSplashScreen : public QSplashScreen
  {
  public:
    const bool takeOwnershipOfStyle;
    SplashscreenStyle* const style;

    CustomSplashScreen(bool takeOwnershipOfStyle, SplashscreenStyle* style)
      : QSplashScreen(style->createPixmap()),
        takeOwnershipOfStyle(takeOwnershipOfStyle),
        style(style)
    {
      getSplashscreenMessageHandler() = std::bind(&CustomSplashScreen::messageHandler, this, std::placeholders::_1);
    }

    ~CustomSplashScreen()
    {
      getSplashscreenMessageHandler() = MessageHandler();
      if(takeOwnershipOfStyle)
        delete style;
    }

    void drawContents(QPainter* painter) override final
    {
      style->paint(painter, message());
    }

    void messageHandler(const QString& message)
    {
      this->showMessage(message);
      repaint();
      qApp->processEvents();
    }
  };

  QSplashScreen* splashscreen = new CustomSplashScreen(takeOwnershipOfStyle, this);

  splashscreen->show();
  QThread::currentThread()->msleep(50);
  qApp->processEvents();

  return splashscreen;
}


/*!
\brief Creates a fallback splashscreen in order not to for the user

\return A new implementation of SplashscreenStyle owned by the caller.
*/
SplashscreenStyle* SplashscreenStyle::createFallbackStyle(const QString& title, const QSize& size, int titleMargin, int messageSize, int messageMargin)
{
  class Fallback final : public SplashscreenStyle
  {
  public:
    const QString title;
    const QSize totalSize;
    const int titleMargin;
    const int messageSize;
    const int messageMargin;

    Fallback(const QString& title, const QSize& totalSize, int titleMargin, int messageSize, int messageMargin)
      : title(title),
        totalSize(totalSize),
        titleMargin(titleMargin),
        messageSize(messageSize),
        messageMargin(messageMargin)
    {
    }

    QPixmap createPixmap() override final
    {
      QPixmap pixmap(totalSize);

      pixmap.fill(QColor::fromRgb(0x2e3436));

      QPainter painter(&pixmap);

      QFont font = painter.font();

      int messageAreaHeight = messageSize+messageMargin*2;

      QRectF titleArea(titleMargin,
                       titleMargin,
                       totalSize.width()-titleMargin*2,
                       totalSize.height()-titleMargin*2-messageAreaHeight);

      QRectF messageBGArea(0,
                           totalSize.height()-messageAreaHeight,
                           totalSize.width(),
                           messageAreaHeight);

      for(int fontSize = titleArea.height(); fontSize>12; fontSize--)
      {
        font.setPixelSize(fontSize);
        painter.setFont(font);

        QRectF boundingRect = painter.boundingRect(titleArea, Qt::AlignCenter, title);

        if(boundingRect.width() < titleArea.width() && boundingRect.height() < titleArea.height())
          break;
      }

      painter.setPen(QPen(QColor::fromRgb(0xf57900)));
      painter.drawText(titleArea, Qt::AlignCenter, title);

      painter.setPen(QPen(Qt::NoPen));
      painter.setBrush(QBrush(QColor::fromRgb(0x232729)));
      painter.drawRect(messageBGArea);

      return pixmap;
    }

    void paint(QPainter* painter, const QString& message) override final
    {
      QRectF messageArea(messageMargin,
                         totalSize.height()-messageSize-messageMargin,
                         totalSize.width()-messageMargin*2,
                         messageSize);

      QFont font = painter->font();
      font.setPixelSize(messageSize);
      painter->setFont(font);
      painter->setPen(QPen(QColor::fromRgb(0x555753)));
      painter->drawText(messageArea, Qt::AlignVCenter|Qt::AlignLeft, message);
    }
  };

  return new Fallback(title, size, titleMargin, messageSize, messageMargin);
}


} // namespace renderer
} // namespace glrt
