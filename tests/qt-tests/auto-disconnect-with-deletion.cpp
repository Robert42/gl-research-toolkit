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


void test_auto_disconnected_subclass_of_qobject()
{
  Foo* foo;
  TemplateFoo<int>* templateObject;
  QMetaObject::Connection connection;

  TemplateFoo<int>::slotWasCalled = 0;
  foo= new Foo();
  templateObject = new TemplateFoo<int>();

  connection = QObject::connect(foo, &Foo::mySignal, templateObject, &TemplateFoo<int>::mySlot);
  EXPECT_TRUE(bool(connection));

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 0);

  foo->mySignal();

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 1);

  EXPECT_TRUE(bool(connection));
  delete templateObject;
  EXPECT_FALSE(bool(connection));

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 1);

  delete foo;

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 1);



  // deleting the other way round;
  TemplateFoo<int>::slotWasCalled = 0;
  foo= new Foo();
  templateObject = new TemplateFoo<int>();

  connection = QObject::connect(foo, &Foo::mySignal, templateObject, &TemplateFoo<int>::mySlot);
  EXPECT_TRUE(bool(connection));

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 0);

  foo->mySignal();

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 1);

  EXPECT_TRUE(bool(connection));
  delete foo;
  EXPECT_FALSE(bool(connection));

  EXPECT_EQ(TemplateFoo<int>::slotWasCalled, 1);

  delete templateObject;

}

}

void main_qt_tests()
{
  int argc;
  char** argv = nullptr;
  QCoreApplication application(argc, argv);

  test_direct_connection_everything_ok();
  test_queued_connection_everything_ok();
  test_auto_disconnected_subclass_of_qobject();
}
