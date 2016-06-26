/// \login.h
/// \brief 游戏界面
/// \author 汪超

#ifndef PLAYGAME_H
#define PLAYGAME_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QTcpSocket>
#include "Protol/Common.h"
#include "Protol/CSProtol.h"
#include "login.h"
#include <QTimer>

namespace Ui
{
class playgame;
}

namespace chess
{
struct SSC_LOGIN_ACK;
struct SSC_TAKESITE_ACK;
struct SPacket;
}

/// \class playgame
/// \游戏界面，负责游戏的主要功能，聊天等
class playgame : public QWidget
{
    Q_OBJECT

public:
    enum
    {
        LINE_END = chess::SSC_PUSH_CHESS_PANEL::LINE_END,
        ROW_END = chess::SSC_PUSH_CHESS_PANEL::ROW_END
    };
    enum
    {
        HEARTBEAT_OVERTIME_SECONDS = 40 // 心跳包超时时间 (s)
    };
    struct SChessPoint
    {
        chess::EPieceTeam m_xTeam;
        chess::EPieceType m_xType;
    };
    /// \fn playgame(QWidget *parent = 0).
    /// \brief 负责进入游戏界面的一些简单操作
    explicit playgame(QWidget *parent = 0);
    /// \fn  ~playgame();
    /// \brief 析构函数
    ~playgame();
    /// \fn void SetCurrentUserName(QString const& qstrUserName).
    /// \param[in] qstrUserName 得到当前进入游戏的玩家名
    /// \return 无
    void SetCurrentUserName(QString const& qstrUserName);
    /// \fn getUserInfo(QString const& username,QString const& password).
    /// \param[in] username.用户名
    /// \param[in] password.密码
    /// \brief 获得登录界面传递的用户信息
    /// \return 无
    void setUserInfo(QString const& username,QString const& password);
protected:
    /// \fn void paintEvent(QPaintEvent *).
    /// \brief 绘制棋盘、棋子等
    /// \return 无
    void paintEvent(QPaintEvent *);
    /// \fn void mousePressEvent(QMouseEvent* e).
    /// \param[in] e.鼠标按下时当前坐标
    /// \brief 获得鼠标按下坐标
    /// \return 无
    void mousePressEvent(QMouseEvent* e);
private slots:
    /// \fn void slot_socket_readyRead().
    /// \brief 读取服务器发送的信息
    /// \return 无
    void slot_socket_readyRead();
    /// \fn void slot_socket_error(QAbstractSocket::SocketError).
    /// \param[in] QAbstractSocket::SocketError.网络出错时，错误原因
    /// \brief 网络连接出错处理
    /// \return 无
    void slot_socket_error(QAbstractSocket::SocketError);
    /// \fn void slot_socket_connected().
    /// \brief 连接到服务器
    /// \return 无
    void slot_socket_connected();
    /// \fn void slot_heartBeatTimeout().
    /// \brief 网络连接上，定时心跳信息包发送
    /// \return 无
    void slot_heartBeatTimeout();
    /// \fn void on_sendButton_clicked().
    /// \brief 聊天时，信息发送
    /// \return 无
    void on_sendButton_clicked();
    /// \fn void on_pushButton_TakeSite_clicked().
    /// \brief 有玩家离开时，玩家坐下处理
    /// \return 无
    void on_pushButton_TakeSite_clicked();
    /// \fn void on_pushButton_ReGame_clicked().
    /// \brief 游戏结束时，重新开始游戏处理
    /// \return 无
    void on_pushButton_ReGame_clicked();

private:
    /// \fn void logicChangeToXY().
    /// \param[in] point.传入逻辑坐标
    /// \brief 将逻辑坐标转化为对于界面实际坐标
    /// \return 返回转化后对于界面实际坐标
    QPoint  logicChangeToXY(const QPoint& point);
    /// \fn void xyChangeToLogic().
    /// \param[in] point.传入界面实际坐标
    /// \brief 将界面实际坐标转化为对于逻辑坐标
    /// \return 返回转化后的逻辑坐标
    QPoint xyChangeToLogic(const QPoint& point);
    /// \fn void  judgeSit(chess::SPacket const & xPack).
    /// \param[in] xPack.服务器发送坐下信息包
    /// \brief 获得坐下结果
    /// \return 无
    void  judgeSit(chess::SPacket const & xPack);
    /// \fn void  initGame(chess::SPacket const & xPack).
    /// \param[in] xPack.服务器发送初始化游戏信息包
    /// \brief 获得游戏初始化时的信息
    /// \return 无
    void initGame(chess::SPacket const& xPacket);
    /// \fn void  judgeLogin(chess::SPacket const & xPack).
    /// \param[in] xPack.服务器发送登录结果信息包
    /// \brief 获得玩家登录结果
    /// \return 无
    void judgeLogin(chess::SPacket const& xPacket);
    /// \fn void  judgeTurn(chess::SPacket const & xPack).
    /// \param[in] xPack.服务器发送该谁行棋信息包
    /// \brief 获得当前行棋玩家
    /// \return 无
    void judgeTurn(chess::SPacket const& xPacket);
    /// \fn void  whoLeave(chess::SPacket const & xPack).
    /// \param[in] xPacket.服务器发送谁离开游戏信息包
    /// \brief 获得当前哪个玩家离开游戏
    /// \return 无
    void whoLeave(chess::SPacket const& xPacket);
    /// \fn void  whoWin(chess::SPacket const & xPacket).
    /// \param[in] xPacket.服务器发送谁获胜信息包
    /// \brief 获得当前哪个玩家获胜
    /// \return 无
    void whoWin(chess::SPacket const& xPacket);
    /// \fn void  movePiece(chess::SPacket const & xPacket).
    /// \param[in] xPacket.服务器发送能否行棋到某位置信息包
    /// \brief 获得当前能否行棋到某位置
    /// \return 无
    void movePiece(chess::SPacket const& xPacket);
    /// \fn void  recvMessage(chess::SPacket const & xPacket).
    /// \param[in] xPacket.接收服务器对手玩家发送消息信息包
    /// \brief 获得对手玩家发送消息
    /// \return 无
    void recvMessage(chess::SPacket const& xPacket);
    /// \fn void  onJiangJun(chess::SPacket const & xPacket).
    /// \param[in] xPacket.服务器发送将军信息包
    /// \brief 获得将军消息
    /// \return 无
    void onJiangJun(chess::SPacket const& xPacket);
    /// \fn void  connectToServer()
    /// \brief 连接到服务器
    /// \return 无
    void connectToServer();
    /// \fn QString GetImageString(SChessPoint const& xChessPoint).
    /// \brief 得到拼接的棋子信息
    /// \return 返回拼接的棋子信息
    QString GetImageString(SChessPoint const& xChessPoint);
private:
    /// \fn void UpdateGameStatus(bool bGameIsRuning).
    /// \brief 更新游戏当前状态，正在游戏，等待游戏
    /// \return 无
    void UpdateGameStatus(bool bGameIsRuning);
private:
    bool m_bForcuseFlag = false;

    QPoint beginPoint;
    QTcpSocket* socket_ = 0;
    QString sendUserName_;
    QString sendPassword_;
    Ui::playgame *ui;
private:
    SChessPoint m_arrChessPanel[LINE_END][ ROW_END];
    chess::EPieceTeam m_xMyTeam = chess::EPieceTeam::None;
    bool m_bIsFirstDisconnect = true;
    bool m_bTurnMe ;
    QTimer *m_xHeartBeatTimer;
};

#endif // PLAYGAME_H
