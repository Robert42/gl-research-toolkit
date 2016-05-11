#include "debugmessage.h"

#include <QMessageBox>
#include <QDebug>

#define USE_MESSAGEBOX 0
#define USE_SIMULATE_CRASH 0

#if USE_MESSAGEBOX
int _isCurrentlyDialogShown = 0;
#endif

#if USE_SIMULATE_CRASH
int _isSimulateCrash = 3;
#endif

bool isCurrentlyDialogShown()
{
#if USE_SIMULATE_CRASH
  if(_isSimulateCrash-- <= 0)
  {
    int* dummy = nullptr;
    dummy[0] = 42;
  }
#endif

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
