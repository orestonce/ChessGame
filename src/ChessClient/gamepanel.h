#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QDialog>
#include <QImage>
#include <QPainter>
#include <QAbstractSocket>

namespace Ui
{
class gamepanel;
}

class QWebSocket;
class GameCore;
class RpcHub;

/// \class GamePanel
/// \游戏界面，负责游戏的主要功能，聊天等
class GamePanel : public QDialog
{
    Q_OBJECT

public:
    /// \fn playgame(QWidget *parent = 0).
    /// \brief 负责进入游戏界面的一些简单操作
    explicit GamePanel(QWidget *parent = 0);
    /// \fn  ~playgame();
    /// \brief 析构函数
    ~GamePanel();
    bool Init(QString const& username,QString const& password);
    void LoadGameCore();
protected:
    /// \fn void paintEvent(QPaintEvent *).
    /// \brief 绘制棋盘、棋子等
    /// \return 无
    void paintEvent(QPaintEvent *);
    /// \fn void mousePressEvent(QMouseEvent* e).
    /// \param[in] e.鼠标按下时当前坐标
    /// \brief 获得鼠标按下坐标
    /// \return 无
    void mousePressEvent(QMouseEvent*);
    void showPopup(QString title, QString msg);
    void showError(QString msg);
    void showErrorAndClose(QString msg);
private slots:
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
private:
    /// \fn void UpdateGameStatus(bool bGameIsRuning).
    /// \brief 更新游戏当前状态，正在游戏，等待游戏
    /// \return 无
    void UpdateGameStatus(bool bGameIsRuning);
private:
    RpcHub* rpchub;
//    QPoint beginPoint;
//    QString sendUserName_;
//    QString sendPassword_;
    int lastPongTime;
//    bool m_bForcuseFlag;
//    bool m_bTurnMe ;
    Ui::gamepanel *ui;
    GameCore *core;
};

#endif // PLAYGAME_H
