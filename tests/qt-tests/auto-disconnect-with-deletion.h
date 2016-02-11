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

template<typename T>
class TemplateFoo : public QObject
{
public:
  static int slotWasCalled;

  void mySlot()
  {
    slotWasCalled++;
  }
};

template<typename T>
int TemplateFoo<T>::slotWasCalled = 0;


} // namespace QObject_disconnectTest
