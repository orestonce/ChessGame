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
#include <memory>
#include <QAbstractSocket>

GamePanel::GamePanel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gamepanel)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    core = new GameCore();
    rpchub = new RpcHub;
    timer = new QTimer(this);

    connect(this, &GamePanel::finished, [&](){
        rpchub->close_websocket();
        rpchub->deleteLater();
    });
}

bool GamePanel::Init(const QString &username, const QString &password, const QString &roomId)
{
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

    auto lastPong = std::make_shared<time_t>(time(NULL));
    connect(timer, &QTimer::timeout, [=](){
        int now = time(NULL);
        int sub = now - *lastPong;
        if (sub > GameCore::HEARTBEAT_OVERTIME_SECONDS) {
            this->showErrorAndClose("服务器长时间无响应.");
            return;
        }
        PingRequest req;
        PingResponse resp;
        bool ok = rpchub->SendAndRecv(req, resp);
        if (ok) {
            *lastPong = now;
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
    ui->pushButton_ReGame->setVisible(false);
    ui->pushButton_TakeSite->setVisible(false);
    LoginRequest req;
    req.Username = username;
    req.Password = password;
    req.RoomId = roomId;
    LoginResponse resp;

    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showErrorAndClose(resp.ErrMsg);
        return false;
    }
    ui->showText->append("登陆服务器成功...") ;
    core->MyUserId = resp.UserId;

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
        QString str=trUtf8("游戏正在运行");
        if (core->IsTurnMe()) {
            str += trUtf8("-该我出手");
        }
        ui->label_GameStatus->setText ( str );
    }
    else
    {
        ui->label_GameStatus->setText ( trUtf8("游戏停止") );
    }
    ui->pushButton_ReGame->setVisible(core->ShowReGame);
    ui->pushButton_TakeSite->setVisible(core->ShowSiteDown);


    const char *turnPicture = NULL;
    const char *down = ":/images/down.png";
    const char *upper= ":/images/up.png";

    if (core->IsBlack())
    {
        ui->upUserLabel->setText(core->WUsername);
        ui->downUserLabel->setText(core->BUsername);
        if (core->IsTurnMe())
        {
            turnPicture = down;
        }
        else
        {
            turnPicture = upper;
        }
    }
    else
    {
        ui->upUserLabel->setText(core->BUsername);
        ui->downUserLabel->setText(core->WUsername);
        if (core->IsTurnW())
        {
            turnPicture = down;
        }
        else
        {
            turnPicture = upper;
        }
    }
    ui->chooselabel->setPixmap(QPixmap(turnPicture));
}

void GamePanel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.drawImage(30, 40, QImage(":/images/XQSTUDIO.png"));
    // 初始化时，将棋子绘制到棋盘对于位置中
    for(int y=0; y<GameCore::MAX_VALUE_Y; ++y)
    {
        for(int x=0; x<GameCore::MAX_VALUE_X; ++x)
        {
            auto point = core->GetPoint(y, x);

            if ( point.IsEmpty() )
            {
                continue;
            }

            QString qstrImagePath = point.GetImageString();
            painter.drawImage(logicChangeToXY(QPoint(x, y)),QImage(qstrImagePath));
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
        QPoint p(it->X, it->Y);
        painter.drawImage(logicChangeToXY(p), QImage(":/images/forcuse.png") );
    }
}

void GamePanel::mousePressEvent(QMouseEvent *e)
{
    QPoint selectPoint = xyChangeToLogic(e->pos());
    PiecePoint piece ;
    piece.Y = selectPoint.y();
    piece.X = selectPoint.x();
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
            this->showError("获取失败");
            return;
        }
        core->SuggestionPointToList = resp.CanMoveToList;
    }
    else if (core->IsPointValied(core->SelectedPointFrom) && core->IsInSuggestionList(piece))
    {
        MovePieceRequest req;
        MovePieceResponse resp;
        req.From.Y = core->SelectedPointFrom.y();
        req.From.X = core->SelectedPointFrom.x();
        req.To = piece;

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
    this->core->SuggestionPointToList.clear();
    this->repaint();
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
    this->rpchub->close_websocket();
    this->timer->stop();
    this->showError(msg);
    this->close();
}

QPoint GamePanel::logicChangeToXY(const QPoint& point)
{
    QPoint p = point;
    //对黑棋进行处理
    if ( core->IsBlack() )
    {
        p.rx () = GameCore::MAX_VALUE_X - p.x () -1 ;
        p.ry () = GameCore::MAX_VALUE_Y - p.y () -1 ;
    }

    p.rx() = (p.x()+1)* 40-3;
    p.ry() = (p.y()+1)* 40+4;

    return p;
}

QPoint GamePanel::xyChangeToLogic(const QPoint& point)
{
    QPoint p;

    p.rx() = (point.x()-8)/40 -1;
    p.ry() = (point.y()+2)/40 -1;
    //对黑棋进行处理
    if ( core->IsBlack() )
    {
        p.rx () = GameCore::MAX_VALUE_X - p.x () -1 ;
        p.ry () = GameCore::MAX_VALUE_Y - p.y ()  -1;
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

QPoint PiecePointToQPoint(PiecePoint p) {
    return QPoint(p.X, p.Y);
}

void assign(QPoint& p1, PiecePoint& p2) {
    p1.rx() = p2.X;
    p1.ry() = p2.Y;
}

void GamePanel::on_pushButton_Chessdb_clicked()
{
    if (!this->core->IsGameRunning) {
        return ;
    }
    if (!this->core->IsTurnMe()) {
        return ;
    }

    QueryChessdbRequest req;
    QueryChessdbResponse resp;
    if (!rpchub->SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty()) {
        this->showError(resp.ErrMsg);
        return;
    }
    if (resp.MoveList.empty()) {
        return;
    }

    assign(core->SelectedPointFrom, resp.MoveList[0].From);
    core->ClearPoint(core->SelectedPointTo);
    core->SuggestionPointToList.clear();
    core->SuggestionPointToList.append(resp.MoveList[0].From);
    core->SuggestionPointToList.append(resp.MoveList[0].To);

    this->repaint();

    MovePieceRequest req1;
    req1.From = resp.MoveList[0].From;
    req1.To = resp.MoveList[0].To;
    MovePieceResponse resp1;
    if (!rpchub->SendAndRecv(req1, resp1) || !resp1.ErrMsg.isEmpty()) {
        this->showError(resp.ErrMsg);
        return;
    }
}
