#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  connect(ui->spinBox_Port, SIGNAL(valueChanged(int)), this, SLOT(update_undo_button()));
  on_toolButton_UndoPort_clicked();

  on_btnStartServer_clicked();
  ui->tabWidget_Games->clear();

}

Dialog::~Dialog()
{
  delete ui;
}

void Dialog::on_btnStartServer_clicked()
{
  server.listen(QHostAddress::LocalHost, ui->spinBox_Port->value());
  update_widgets_to_connection_state();
}

void Dialog::on_btnStopServer_clicked()
{
  server.close();
  update_widgets_to_connection_state();
}

void Dialog::update_widgets_to_connection_state()
{
  ui->btnStartServer->setEnabled(!server.isListening());
  ui->btnStopServer->setEnabled(server.isListening());
  ui->spinBox_Port->setEnabled(!server.isListening());
}

void Dialog::on_toolButton_UndoPort_clicked()
{
  ui->spinBox_Port->setValue(GLRT_PROFILER_DEFAULT_PORT);
}

void Dialog::update_undo_button()
{
  ui->toolButton_UndoPort->setVisible(ui->spinBox_Port->value() != GLRT_PROFILER_DEFAULT_PORT);
}
