#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = 0);
  ~Dialog();

private:
  Ui::Dialog *ui;

  QTcpServer server;

private slots:
  void on_btnStartServer_clicked();
  void on_btnStopServer_clicked();
  void update_widgets_to_connection_state();
};

#endif // DIALOG_H
