/// \login.h
/// \brief 登录界面
/// \author 汪超

#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>

namespace Ui
{
  class Login;
}

namespace chess
{
  struct SSC_LOGIN_ACK;
}
class Login : public QMainWindow
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = 0);
    ~Login();
private slots:
    void on_loginButton_clicked();
    void on_registerButton_clicked();
    void on_setServerButton_clicked();
    void on_cancelButton_clicked();
private:
    void loadStatusFromDB();
    void saveStatusToDB();
private:
    Ui::Login* ui;
};
#endif // LOGIN_H
