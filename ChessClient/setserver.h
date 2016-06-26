#ifndef SETSERVER_H
#define SETSERVER_H
#include <QWidget>
namespace Ui
{
class SetServer;
}

/// \class SetServer
/// \设置连接服务器地址，并保存
class SetServer : public QWidget
{
    Q_OBJECT
public:
    /// \fn SetServer(QWidget *parent = 0).
    /// \brief 负责初始化界面时的一些操作
    explicit SetServer(QWidget *parent = 0);
    /// \fn  ~SetServer().
    /// \brief 析构函数
    ~SetServer();
    /// \fn void rememberAddress().
    /// \brief 将连接服务器地址保存到本地文件中
    /// \return 无
    void rememberAddress();
    /// \fn void rememberPort().
    /// \brief 将连接服务器端口保存到本地文件中
    /// \return 无
    void rememberPort();
    /// \fn void readingAddress().
    /// \brief 将连接服务器的地址动态的显示在窗口中
    /// \return 无
    void readingAddress();
    /// \fn void readingPort().
    /// \brief 将连接服务器的端口动态的显示在窗口中
    /// \return 无
    void readingPort();
    /// \fn QString sendIP().
    /// \brief 将连接服务器的IP传递到其他类中，用于网络连接
    /// \return 无
    QString sendIP();
    /// \fn QString sendPort().
    /// \brief 将连接服务器的IP传递到其他类中，用于网络连接
    /// \return 无
    int sendPort();
private slots:
    /// \fn void on_recoveryButton_clicked().
    /// \brief 点击恢复默认按钮，设置了一个默认的服务连接地址和端口
    /// \return 无
    void on_recoveryButton_clicked();
    /// \fn void on_confirmButton_clicked().
    /// \brief 判断点击确认时，服务器地址和端口状态并跳转到登录界面
    /// \return 无
    void on_confirmButton_clicked();
    /// \fn void on_confirmButton_clicked().
    /// \brief 判断点击取消时跳转到登录界面
    /// \return 无
    void on_cancelButton_clicked();
    /// \fn void paintEvent(QPaintEvent *).
    /// \brief 绘制背景图片
    /// \return 无
    void paintEvent(QPaintEvent *);
private:
    QString address_ ;
    QString port_ ;
    QString default_address = "103.39.79.210";
    QString default_port = "10010";
    Ui::SetServer *ui;
};
#endif // SETSERVER_H
