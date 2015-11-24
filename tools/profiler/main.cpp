#include <QApplication>

#include "dialog.h"

int main(int argc, char** argv)
{
  QApplication application(argc, argv);
  Q_UNUSED(application);

  Dialog dialog;

  return dialog.exec();
}
