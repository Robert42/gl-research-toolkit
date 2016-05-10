#include "debugmessage.h"

#include <QMessageBox>

void debugMessage(const QString& title, const QString& message)
{
  QMessageBox::information(nullptr, title, message);
}
