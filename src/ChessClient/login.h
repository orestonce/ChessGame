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
/// \class Login
/// \brief 登录界面，负责用户名、密码等信息输入，并传递
class Login : public QMainWindow
{
    Q_OBJECT
public:
    /// \fn Login(QWidget *parent = 0).
    /// \brief 若记住密码为true，则保存此次登录的用户名和密码，否则清空之前保存的用户名和密码
    explicit Login(QWidget *parent = 0);
    /// \fn ~Login().
    /// \fn 析构函数
    ~Login();
private slots:
    /// \fn void on_loginButton_clicked().
    /// \brief 点击登录按钮，检查用户信息，并传递
    /// \return 无
    void on_loginButton_clicked();
    /// \fn void on_registerButton_clicked().
    /// \brief 点击注册按钮，进入注册用户界面
    /// \return 无
    void on_registerButton_clicked();
    /// \fn void on_setServerButton_clicked().
    /// \brief 点击设置服务器界面，设置连接的服务器
    /// \return 无
    void on_setServerButton_clicked();
//    void slot_socket_readyRead();
//    void sendUserInfo();
    /// \fn on_cancelButton_clicked()
    /// \brief 点击取消按钮，退出程序
    /// \return 无
    void on_cancelButton_clicked();
private:
    Ui::Login* ui;
};
#endif // LOGIN_H
