#ifndef REGISTER_H
#define REGISTER_H
#include <QWidget>
#include <QTcpSocket>
namespace Ui
{
class Register;
}
/// \class Register
/// \注册用户，主要将用户注册信息发送给服务器，并显示注册结果
class Register : public QWidget
{
    Q_OBJECT
public:
    /// \fn Register(QWidget *parent = 0).
    /// \brief 负责初始化界面时的一些操作
    explicit Register(QWidget *parent = 0);
    /// \fn ~Register().
    /// \brief 析构函数
    ~Register();
private:
    /// \fn void connectToServer().
    /// \brief 连接服务器
    /// \return 无
    void connectToServer();
private slots:
    /// \fn void on_confirmButton_clicked().
    /// \brief 用户点击提交注册按钮后相应操作
    /// \return 无
    void on_confirmButton_clicked();
    /// \fn void sendUserInfo().
    /// \brief 将用户注册信息发送给服务器端
    /// \return 无
    void sendUserInfo();
    /// \fn void on_socket_readyRead().
    /// \brief 接收到服务器登录结果，并显示给用户
    /// \return 无
    void on_socket_readyRead();
    /// \fn void on_socket_error(QAbstractSocket::SocketError).
    /// \brief 网络连接错误
    /// \return 无
    void on_socket_error(QAbstractSocket::SocketError);
    /// \fn void paintEvent(QPaintEvent *).
    /// \brief 绘制背景图片
    /// \return 无
    void paintEvent(QPaintEvent *);

    void on_canelButton_clicked();

private:
    QString username_;
    QString password_;
    QString confirm_password_;
    QTcpSocket *socket_ = 0;
    bool socketFlag = false;
    Ui::Register *ui;
};
#endif // REGISTER_H
