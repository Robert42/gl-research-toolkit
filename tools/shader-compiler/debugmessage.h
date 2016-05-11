#ifndef DEBUGMESSAGE_H
#define DEBUGMESSAGE_H

#include <QString>

void debugMessage(const QString& title, const QString& message);

bool isCurrentlyDialogShown();

#endif // DEBUGMESSAGE_H
