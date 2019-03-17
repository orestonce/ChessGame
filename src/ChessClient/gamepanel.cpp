#include "gamepanel.h"
#include "gamecore.h"
#include "login.h"
#include "ui_gamepanel.h"
#include "CSProtocol.h"
#include "database.h"
#include "rpchub.h"
#include <QPoint>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QTcpSocket>
#include <QPixmap>
#include <QScrollBar>
#include <QTimer>
#include <QWebSocket>
#include <time.h>
#include <QAbstractSocket>

GamePanel::GamePanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gamepanel)
{
    ui->setupUi(this);
    core = new GameCore();
    rpchub = new RpcHub;

    connect(this, &GamePanel::finished, [&](){
        rpchub->close_websocket();
        rpchub->deleteLater();
    });

    this->setStyleSheet("QMainWindow {border-image:url(:/images/back6.jpg)}");

    ui->pushButton_TakeSite->hide ();
    ui->pushButton_ReGame->hide ();

    ui->chooselabel->setAlignment(Qt::AlignCenter);
    ui->upUserLabel->setAlignment(Qt::AlignCenter);
    ui->downUserLabel->setAlignment(Qt::AlignCenter);

    ui->showText->setVerticalScrollBar(ui->showTextBar);
}

bool GamePanel::Init(const QString &username, const QString &password)
{
    QTimer *timer = new QTimer;
    lastPongTime = time(NULL);

    core->MyUsername = username;
    this->setWindowTitle(this->windowTitle() + "-" + username);
    rpchub->RegisterSyncPanelMessage([=](const SyncPanelMessage& message){
        core->Load(message);
        this->LoadGameCore();
        this->repaint();
    });
    rpchub->RegisterServerKickYou([=](const ServerKickYou& msg){
        rpchub->RemoveAll();
        this->showErrorAndClose(msg.ErrMsg);
    });

    timer->start(1000 * GameCore::HEARTBEAT_OVERTIME_SECONDS/2);
    connect(timer, &QTimer::timeout, [=](){
        int now = time(NULL);
        int sub = now - lastPongTime;
        if (sub > GameCore::HEARTBEAT_OVERTIME_SECONDS) {
            this->showErrorAndClose("服务器长时间无响应.");
            return;
        }
        PingRequest req;
        PingResponse resp;
        bool ok = rpchub->SendAndRecv(req, resp);
        if (ok) {
            lastPongTime = now;
        }
    });
    connect(this, &GamePanel::finished, [=](){
        timer->stop();
    });

    bool ok = rpchub->InitWebsocket(Database::GetInstance()->GetServerAddr(), 10);
    if (!ok) {
        this->showErrorAndClose("链接服务器失败");
        return false;
    }
    rpchub->RegisterChatMessage([=](const ChatMessage& message){
        this->ui->showText->append(message.Username+" ["+message.TimeStr + "] " + message.Text);
        this->repaint();
    });
    ui->showText->append("链接服务器成功...");
    LoginRequest req;
    req.Username = username;
    req.Password = password;
    req.RoomId = "room-id";
    LoginResponse resp;

    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showErrorAndClose(resp.ErrMsg);
        return false;
    }
    ui->showText->append("登陆服务器成功...") ;

    connect(rpchub, &RpcHub::signal_websocket_error, [=](QString errMsg){
        this->showErrorAndClose(errMsg);
    });
    rpchub->RegisterGameOverNotice([=](const GameOverNotice& message) {
        QString text;
        if (message.IsPeace) {
            text = "和棋";
        } else {
            text = "胜利者是 " + message.WinUsername;
        }
        this->showPopup("游戏结束", text);
    });
    return true;
}

GamePanel::~GamePanel()
{
    delete ui;
    delete core;
}

void GamePanel::LoadGameCore()
{
    if ( core->IsGameRunning )
    {
        ui->label_GameStatus->setText ( trUtf8("游戏正在运行") );
    }
    else
    {
        ui->label_GameStatus->setText ( trUtf8("游戏停止") );
    }
    ui->pushButton_ReGame->setVisible(core->ShowReGame);
    ui->pushButton_TakeSite->setVisible(core->ShowSiteDown);


    const char *turnUsername = NULL;
    const char *down = ":/images/down.png";
    const char *upper= ":/images/up.png";

    if (core->AmUpper())
    {
        ui->upUserLabel->setText(core->DownUsername);
        ui->downUserLabel->setText(core->UpperUsername);
        if (core->IsTurnMe())
        {
            turnUsername = down;
        }
        else
        {
            turnUsername = upper;
        }
    }
    else
    {
        ui->upUserLabel->setText(core->UpperUsername);
        ui->downUserLabel->setText(core->DownUsername);
        if (core->IsTurnUpper())
        {
            turnUsername = upper;
        }
        else
        {
            turnUsername = down;
        }
    }
    ui->chooselabel->setPixmap(QPixmap(turnUsername));
}

void GamePanel::paintEvent(QPaintEvent *)
{
    //绘制游戏界面背景
    QPixmap pixmap = QPixmap(":/images/back5.jpg").scaled(this->size());
    QPainter painterBackground(this);
    painterBackground.drawPixmap(this->rect(),pixmap);

    QPainter painter(this);

    painter.drawImage(30, 40, QImage(":/images/XQSTUDIO.png"));
    // 初始化时，将棋子绘制到棋盘对于位置中
    for(int iLine=0; iLine<GameCore::LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<GameCore::COLUMN_END; ++iRow)
        {
            auto point = core->GetPoint(iLine, iRow);

            if ( point.IsEmpty() )
            {
                continue;
            }

            QString qstrImagePath = point.GetImageString();
            painter.drawImage(logicChangeToXY(QPoint(iRow, iLine)),QImage(qstrImagePath));
        }
    }

    if ( core->IsPointValied(core->SelectedPointFrom) )
    {
        painter.drawImage(logicChangeToXY(core->SelectedPointFrom), QImage(":/images/forcuse.png") );
    }
    if ( core->IsPointValied(core->SelectedPointTo) )
    {
        painter.drawImage(logicChangeToXY(core->SelectedPointTo), QImage(":/images/forcuse.png") );
    }
    for (auto it = core->SuggestionPointToList.begin(); it != core->SuggestionPointToList.end(); it++)
    {
        QPoint p(it->Column, it->Line);
        painter.drawImage(logicChangeToXY(p), QImage(":/images/forcuse.png") );
    }
}

void GamePanel::mousePressEvent(QMouseEvent *e)
{
    QPoint selectPoint = xyChangeToLogic(e->pos());
    PiecePoint piece ;
    piece.Line = selectPoint.y();
    piece.Column = selectPoint.x();
    if ( !core->IsPointValied(selectPoint) || !core->IsTurnMe() )
    {
        return ;
    }
    if (core->IsPointValied(selectPoint) && core->SameTeamWithMe(selectPoint))
    {
        core->SelectedPointFrom = selectPoint;
        core->ClearPoint(core->SelectedPointTo);
        GetSuggestionRequest req;
        GetSuggestionResponse resp;
        req.FromPoint = piece;
        if (!rpchub->SendAndRecv(req, resp) )
        {
            this->showError("");
        } else {
            core->SuggestionPointToList = resp.CanMoveToList;
        }
    }
    else if (core->IsPointValied(core->SelectedPointFrom) && core->IsInSuggestionList(piece))
    {
        MovePieceRequest req;
        MovePieceResponse resp;
        req.FromPoint.Line = core->SelectedPointFrom.y();
        req.FromPoint.Column = core->SelectedPointFrom.x();
        req.ToPoint = piece;

        if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty())
        {
            this->showError(resp.ErrMsg);
            return ;
        }
        core->ClearPoint(core->SelectedPointFrom);
        core->ClearPoint(core->SelectedPointTo);
        core->SuggestionPointToList.clear();
    }
    else
    {
        core->ClearPoint(core->SelectedPointFrom);
        core->ClearPoint(core->SelectedPointTo);
        core->SuggestionPointToList.clear();
    }
    repaint();
}

void GamePanel::showPopup(QString title, QString msg)
{
    if (!this->isVisible()) {
        return;
    }

    rpchub->blockSignals(true);
    QMessageBox::about(this, title, msg);
    rpchub->blockSignals(false);
}

void GamePanel::showError(QString msg)
{
    if (msg.isEmpty()) {
        msg = "未知错误";
    }
    this->showPopup("错误", msg);
}

void GamePanel::showErrorAndClose(QString msg)
{
    this->showError(msg);
    this->close();
}

QPoint GamePanel::logicChangeToXY(const QPoint& point)
{

    QPoint p = point;
    //对黑棋进行处理
    if ( core->AmUpper() )
    {
        p.rx () = GameCore::COLUMN_END - p.x () -1 ;
        p.ry () = GameCore::LINE_END - p.y () -1 ;
    }

    p.rx() = (p.x()+1)* 40 -  3;
    p.ry() = (p.y()+1)* 40 + 4;

    return p;
}

QPoint GamePanel::xyChangeToLogic(const QPoint& point)
{
    QPoint p;

    p.rx() = (point.x() -  8)/40 -1 ;
    p.ry() = (point.y() +2)/40 -1 ;
    //对黑棋进行处理
    if ( core->AmUpper() )
    {
        p.rx () = GameCore::COLUMN_END - p.x () -1 ;
        p.ry () = GameCore::LINE_END - p.y ()  -1;
    }

    return p;
}

void GamePanel::on_sendButton_clicked()
{
    QString context = ui->sendEdit->toPlainText();
    if(context.isEmpty())
    {
        this->showError("发送的内容不能为空");
        return ;
    }
    ui->sendEdit->clear();
    ChatRequest req;
    req.Text = context;
    ChatResponse resp;

    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showError(resp.ErrMsg);
    }
}

void GamePanel::on_pushButton_TakeSite_clicked()
{
    TakeSiteRequest req;
    TakeSiteResponse resp;

    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showError(resp.ErrMsg);
    }
}

void GamePanel::on_pushButton_ReGame_clicked()
{
    ReGameRequest req;
    ReGameResponse resp;
    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showError(resp.ErrMsg);
    }
}

