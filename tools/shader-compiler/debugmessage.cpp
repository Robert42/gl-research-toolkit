#include "debugmessage.h"

#include <QMessageBox>
#include <QDebug>

#define USE_QT_DIALOG 0
#define USE_SIMULATE_CRASH 0

#if USE_QT_DIALOG
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

#if USE_QT_DIALOG
  return _isCurrentlyDialogShown > 0;
#else
  return false;
#endif
}
