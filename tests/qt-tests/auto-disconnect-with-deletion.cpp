#include <testing-framework.h>
#include <QCoreApplication>
#include "auto-disconnect-with-deletion.h"

using namespace QObject_disconnectTest;

namespace QObject_disconnectTest
{

void test_direct_connection_everything_ok()
{
  Foo foo;
  Foo bar;

  QObject::connect(&foo, SIGNAL(mySignal()), &bar, SLOT(mySlot()), Qt::DirectConnection);

  foo.mySignal();

  EXPECT_EQ(bar.slotWasCalled, true);
}

void test_queued_connection_everything_ok()
{
  Foo foo;
  Foo bar;

  QObject::connect(&foo, SIGNAL(mySignal()), &bar, SLOT(mySlot()), Qt::QueuedConnection);

  foo.mySignal();

  EXPECT_EQ(bar.slotWasCalled, false);

  qApp->processEvents();

  EXPECT_EQ(bar.slotWasCalled, true);
}

void test_queued_connection_everything_deleting_receiver_in_between()
{
  Foo foo;

  Foo* bar = new Foo();

  QObject::connect(&foo, SIGNAL(mySignal()), bar, SLOT(mySlot()), Qt::QueuedConnection);

  foo.mySignal();

  EXPECT_EQ(bar->slotWasCalled, false);

  delete bar;

  qApp->processEvents();
}

void test_queued_connection_everything_deleting_sender_in_between()
{
  Foo* foo = new Foo();
  Foo* bar = new Foo();

  QObject::connect(foo, SIGNAL(mySignal()), bar, SLOT(mySlot()), Qt::QueuedConnection);

  foo->mySignal();

  EXPECT_EQ(bar->slotWasCalled, false);

  delete foo;

  qApp->processEvents();

  EXPECT_EQ(bar->slotWasCalled, false);
}


}

void main_qt_tests()
{
  int argc;
  char** argv = nullptr;
  QCoreApplication application(argc, argv);

  test_direct_connection_everything_ok();
  test_queued_connection_everything_ok();
}
