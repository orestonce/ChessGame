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

class GamePanel : public QDialog
{
    Q_OBJECT

public:
    explicit GamePanel(QWidget *parent = 0);
    ~GamePanel();
    bool Init(QString const& username, QString const& password, const QString &roomId);
    void LoadGameCore();
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent*);

    void showPopup(QString title, QString msg);
    void showError(QString msg);
    void showErrorAndClose(QString msg);
private slots:
    void on_sendButton_clicked();
    void on_pushButton_TakeSite_clicked();
    void on_pushButton_ReGame_clicked();

private:
    QPoint logicChangeToXY(const QPoint& point);
    QPoint xyChangeToLogic(const QPoint& point);
private:
    void UpdateGameStatus(bool bGameIsRuning);
private:
    RpcHub* rpchub;
    int lastPongTime;
    Ui::gamepanel *ui;
    GameCore *core;
};

#endif // PLAYGAME_H
