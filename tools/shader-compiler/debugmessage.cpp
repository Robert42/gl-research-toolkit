#include "debugmessage.h"

#include <QMessageBox>

#define USE_MESSAGEBOX 0

int _isCurrentlyDialogShown = 0;

bool isCurrentlyDialogShown()
{
#if USE_MESSAGEBOX
  return _isCurrentlyDialogShown > 0;
#else
  return false;
#endif
}

void debugMessage(const QString& title, const QString& message)
{
#if USE_MESSAGEBOX
  _isCurrentlyDialogShown++;
  QMessageBox::information(nullptr, title, message);
  _isCurrentlyDialogShown--;
#endif
}
