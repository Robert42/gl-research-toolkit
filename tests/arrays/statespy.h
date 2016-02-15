#ifndef STATESPY_H
#define STATESPY_H

#include <glrt/toolkit/array.h>

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

  static void clearLog();
  static void clearIndex(int nextIndex=0);
  static QString log();
  static bool enabledDebugPrint;

private:
  static int _nextIndex;

  static int nextIndex();
  static QString& globalState();
};

typedef glrt::Array<StateSpy, glrt::ArrayTraits_cCmCmOD<StateSpy, glrt::ArrayCapacityTraits_Capacity_Blocks<2, 4>>> StateSpyArray;
typedef glrt::Array<StateSpy, glrt::ArrayTraits_cCmCmOD<StateSpy, glrt::ArrayCapacityTraits_Capacity_Blocks<2, 2>>> StateSpyArray_CapacityBock_2_2;

#endif // STATESPY_H
