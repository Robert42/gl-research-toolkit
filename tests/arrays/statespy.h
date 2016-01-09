#ifndef STATESPY_H
#define STATESPY_H

#include <QString>
#include <QMap>

class StateSpy
{
public:
  struct EnablePrinting
  {
    EnablePrinting(){enabledDebugPrint=true;}
    EnablePrinting(const EnablePrinting&)=delete;
    EnablePrinting& operator=(const EnablePrinting&)=delete;
    ~EnablePrinting(){enabledDebugPrint=false;}
  };

  int index;

  StateSpy();
  StateSpy(const StateSpy& s);
  StateSpy(StateSpy&& s);

  ~StateSpy();

  StateSpy& operator=(const StateSpy& s);
  StateSpy& operator=(StateSpy&& s);

  void print(const QString& s);

  static void clear();
  static QString log();
  static bool enabledDebugPrint;

private:
  static int _nextIndex;

  static int nextIndex();
  static QString& globalState();
};

#endif // STATESPY_H
