#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  ui->spinBox_Port->setValue(GLRT_PROFILER_DEFAULT_PORT);

  on_btnStartServer_clicked();
}

Dialog::~Dialog()
{
  delete ui;
}

void Dialog::on_btnStartServer_clicked()
{
  ui->btnStartServer->setEnabled(false);
  ui->btnStopServer->setEnabled(true);
  ui->spinBox_Port->setEnabled(false);
  server.listen(QHostAddress::LocalHost, ui->spinBox_Port->value());
}

void Dialog::on_btnStopServer_clicked()
{
  ui->btnStartServer->setEnabled(true);
  ui->btnStopServer->setEnabled(false);
  ui->spinBox_Port->setEnabled(true);
  server.close();
}
