#include <QObject>

namespace QObject_disconnectTest {

class Foo : public QObject
{
Q_OBJECT

public:
  int slotWasCalled = 0;

public slots:
  void mySlot()
  {
    slotWasCalled++;
  }

signals:
  void mySignal();
};


} // namespace QObject_disconnectTest
