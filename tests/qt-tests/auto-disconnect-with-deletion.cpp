#include <gtest/gtest.h>
#include <QCoreApplication>
#include "auto-disconnect-with-deletion.h"

using namespace QObject_disconnectTest;

namespace QObject_disconnectTest
{

class qt_tests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    int argc;
    char** argv = nullptr;
    application = new QCoreApplication(argc, argv);
  }

  void TearDown() override
  {
    delete application;
  }

  QCoreApplication* application;
};

TEST_F(qt_tests, direct_connection_everything_ok)
{
  Foo foo;
  Foo bar;

  QObject::connect(&foo, SIGNAL(mySignal()), &bar, SLOT(mySlot()), Qt::DirectConnection);

  foo.mySignal();

  EXPECT_EQ(bar.slotWasCalled, true);
}

TEST_F(qt_tests, queued_connection_everything_ok)
{
  Foo foo;
  Foo bar;

  QObject::connect(&foo, SIGNAL(mySignal()), &bar, SLOT(mySlot()), Qt::QueuedConnection);

  foo.mySignal();

  EXPECT_EQ(bar.slotWasCalled, false);

  qApp->processEvents();

  EXPECT_EQ(bar.slotWasCalled, true);
}

TEST_F(qt_tests, queued_connection_everything_deleting_receiver_in_between)
{
  Foo foo;

  Foo* bar = new Foo();

  QObject::connect(&foo, SIGNAL(mySignal()), bar, SLOT(mySlot()), Qt::QueuedConnection);

  foo.mySignal();

  EXPECT_EQ(bar->slotWasCalled, false);

  delete bar;

  qApp->processEvents();

  EXPECT_EQ(bar->slotWasCalled, false);
}

TEST_F(qt_tests, queued_connection_everything_deleting_sender_in_between)
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


TEST_F(qt_tests, auto_disconnected_subclass_of_qobject)
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

