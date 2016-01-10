#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  ui->spinBox_Port->setValue(GLRT_PROFILER_DEFAULT_PORT);
}

Dialog::~Dialog()
{
  delete ui;
}
