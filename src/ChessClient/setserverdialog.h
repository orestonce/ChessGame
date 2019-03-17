#ifndef SETSERVERDIALOG_H
#define SETSERVERDIALOG_H

#include <QDialog>

namespace Ui {
class SetServerDialog;
}

class SetServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetServerDialog(QWidget *parent = 0);
    ~SetServerDialog();

    void saveServerAddress();
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_reset_clicked();

private:
    void loadServerAddress();
    Ui::SetServerDialog *ui;
};

#endif // SETSERVERDIALOG_H
