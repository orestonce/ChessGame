#include "playgame.h"
#include "login.h"
#include "setserver.h"
#include "ui_playgame.h"
#include "Protol/CSProtol.h"
#include "Protol/Common.h"
#include "Protol/CSProtol.Parser.hpp"
#include "Protol/CSProtol.Serialize.hpp"
#include <QDebug>
#include <QPoint>
#include <QTcpSocket>
#include <QMouseEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QTcpSocket>
#include <QPixmap>
#include <QScrollBar>

playgame::playgame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::playgame)
{
    ui->setupUi(this);

    //this->setStyleSheet("QMainWindow {border-image:url(:/images/images/back6.jpg)}");

    ui->pushButton_TakeSite->hide ();
    ui->pushButton_ReGame->hide ();

    ui->chooselabel->setAlignment(Qt::AlignCenter);
    ui->upUserLabel->setAlignment(Qt::AlignCenter);
    ui->downUserLabel->setAlignment(Qt::AlignCenter);

    ui->showText->setVerticalScrollBar(ui->showTextBar);
    ui->showText->setReadOnly(true);

    m_xHeartBeatTimer = new QTimer(this);
    connect(m_xHeartBeatTimer, SIGNAL(timeout()),this, SLOT(slot_heartBeatTimeout()));
    m_xHeartBeatTimer->start(1000 * (HEARTBEAT_OVERTIME_SECONDS/2) );

    connectToServer();
    connect(socket_,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slot_socket_error(QAbstractSocket::SocketError)));
    //对用来接收棋子初始化信息的二维数组进行空值处理
    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            m_arrChessPanel[iLine][iRow].m_xTeam = chess::EPieceTeam::None;
            m_arrChessPanel[iLine][iRow].m_xType = chess::EPieceType::None;
        }
    }

    UpdateGameStatus (false);
}
playgame::~playgame()
{
    Login *login = new Login;
    login->show();
    delete ui;
}

void playgame::paintEvent(QPaintEvent *)
{
    //绘制游戏界面背景
    QPixmap pixmap = QPixmap(":/images/images/back5.jpg").scaled(this->size());
    QPainter painterBackground(this);
    painterBackground.drawPixmap(this->rect(),pixmap);

    QPainter painter(this);

    painter.drawImage(30, 40, QImage(":/images/images/XQSTUDIO.png"));
    // 初始化时，将棋子绘制到棋盘对于位置中
    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            auto const& xPiecePoint = m_arrChessPanel[iLine][iRow];

            if ( xPiecePoint.m_xTeam == chess::EPieceTeam::None ||
                 xPiecePoint.m_xType == chess::EPieceType::None )
            {
                continue;
            }

            QString qstrImagePath = GetImageString(xPiecePoint);
            painter.drawImage(logicChangeToXY(QPoint(iRow, iLine)),QImage(qstrImagePath));
        }
    }
    //在选中己方棋子时，绘制一个选中标志
    if ( m_bForcuseFlag )
    {
        painter.drawImage(logicChangeToXY(beginPoint), QImage(":/images/images/forcuse.png") );
    }
}

void playgame::mousePressEvent(QMouseEvent *e)
{

    QPoint selectPoint = xyChangeToLogic(e->pos());

    qDebug() << selectPoint << e->pos () << logicChangeToXY (selectPoint);
    qDebug() << beginPoint ;
    if ( selectPoint.x ()<0 || selectPoint.x() >=ROW_END ||
         selectPoint.y () <0 || selectPoint.y () >=LINE_END ||
         !m_bTurnMe )
    {
        return ;
    }

    if(m_bForcuseFlag == true)
    {
        if(m_xMyTeam == m_arrChessPanel[selectPoint.y()][selectPoint.x()].m_xTeam)
        {
            beginPoint = selectPoint;
        }
        else
        {
            std::string strOutput;

            chess::SCS_MOVE_PIECE xMovePiece;

            xMovePiece.m_xFrom.m_iLine = beginPoint.y();
            xMovePiece.m_xFrom.m_iRow = beginPoint.x();
            xMovePiece.m_xTo.m_iLine = selectPoint.y();
            xMovePiece.m_xTo.m_iRow = selectPoint.x();

            chess::SerializeToString(xMovePiece, chess::CS_MOVE_PIECE, strOutput);

            socket_->write(&strOutput[0], strOutput.size());
            m_bForcuseFlag = false;
        }
    }
    else
    {
        if ( m_xMyTeam == m_arrChessPanel[selectPoint.y()][selectPoint.x()].m_xTeam )
        {
            m_bForcuseFlag = true;
            beginPoint = selectPoint;
        }
    }
    repaint();
}

void playgame::SetCurrentUserName(const QString &qstrUserName)
{
    sendUserName_ = qstrUserName;
}

void playgame::setUserInfo(const QString &username, const QString &password)
{
    sendUserName_ = username;
    sendPassword_ = password;
}

QPoint playgame::logicChangeToXY(const QPoint& point)
{

    QPoint p = point;
    //对黑棋进行处理
    if ( m_xMyTeam == chess::EPieceTeam::Up )
    {
        p.rx () = ROW_END - p.x () -1 ;
        p.ry () = LINE_END - p.y () -1 ;
    }

    p.rx() = (p.x()+1)* 40 -  3;
    p.ry() = (p.y()+1)* 40 + 4;

    return p;
}

QPoint playgame::xyChangeToLogic(const QPoint& point)
{
    QPoint p;

    p.rx() = (point.x() -  8)/40 -1 ;
    p.ry() = (point.y() +2)/40 -1 ;
    //对黑棋进行处理
    if ( m_xMyTeam == chess::EPieceTeam::Up )
    {
        p.rx () = ROW_END - p.x () -1 ;
        p.ry () = LINE_END - p.y ()  -1;
    }

    return p;
}

void playgame::judgeSit(const chess::SPacket &xPack)
{
    struct chess::SSC_TAKESITE_ACK sit;
    if ( !chess::GetStruct(xPack.pParamBegin, xPack.pParamEnd, sit) )
    {
        return ;
    }

    switch ( static_cast<chess::ETakeSiteResult>( sit.m_uTakeSiteResult) )
    {
    case chess::ETakeSiteResult::TakeSiteOK :
    {
        if ( sit.m_szUserName == sendUserName_.toStdString() )
        {
            m_xMyTeam = static_cast<chess::EPieceTeam>( sit.m_uDestSite );
            ui->pushButton_TakeSite->hide ();
        }
        else
        {
            qDebug() << sit.m_szUserName << sendUserName_ ;
        }

        switch ( static_cast<chess::EPieceTeam>(sit.m_uDestSite) )
        {
        case chess::EPieceTeam::Down:
            ui->downUserLabel->setText(sit.m_szUserName);
            break;
        case chess::EPieceTeam::Up:
            ui->upUserLabel->setText(sit.m_szUserName);
            break;
        default:
            break;
        }

        QMessageBox::about(this, trUtf8 ("坐下成功"), sit.m_szUserName + trUtf8 ("坐下成功 !") );
    }
        break;


    default:
        break;
    }
}

void playgame::initGame(const chess::SPacket &xPacket)
{

    chess::SSC_PUSH_CHESS_PANEL xChessPanel;
    //将重新开始游戏按钮隐藏
    ui->pushButton_ReGame->hide ();

    if ( !chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xChessPanel) )
    {
        return ;
    }

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            m_arrChessPanel[iLine][iRow].m_xType = static_cast<chess::EPieceType>( xChessPanel.m_arr[iLine][iRow].m_uChessType);
            m_arrChessPanel[iLine][iRow].m_xTeam = static_cast<chess::EPieceTeam>( xChessPanel.m_arr[iLine][iRow].m_uChessTeam);
        }
    }

    if(m_xMyTeam == chess::EPieceTeam::Up)
    {
        ui->downUserLabel->setText(xChessPanel.m_szUpUserName);
        ui->upUserLabel->setText(xChessPanel.m_szDownUserName);
    }
    else
    {
        ui->upUserLabel->setText( xChessPanel.m_szUpUserName );
        ui->downUserLabel->setText( xChessPanel.m_szDownUserName );
    }
    //更新游戏的当前状态
    UpdateGameStatus(xChessPanel.m_bIsRuning);

    m_bTurnMe = false;

    repaint();
}

void playgame::judgeLogin(const chess::SPacket &xPacket)
{
    chess::SSC_LOGIN_ACK xLogin;
    if( !chess::GetStruct(xPacket.pParamBegin,xPacket.pParamEnd,xLogin))
    {
        return;
    }
    switch (xLogin.m_uLoginResult)
    {
    case chess::SSC_LOGIN_ACK::LOGIN_OK:
        QMessageBox::information(this,NULL,trUtf8("登录成功！！"),QMessageBox::Ok);
        break;
    case chess::SSC_LOGIN_ACK::PASSWORD_ERROR:
    {
        QMessageBox::information(this,NULL,trUtf8("用户名和密码不匹配，请重新输入！！"),QMessageBox::Ok);
        Login * login = new Login;
        login->show();
        close();
    }
        break;
    default:
        QMessageBox::information(this,NULL,trUtf8("登录失败！！"),QMessageBox::Ok);
        Login * login = new Login;
        login->show();
        close();
        break;
    }
}
void playgame::judgeTurn(const chess::SPacket &xPacket)
{
    chess::SSC_PUSH_TURN_CHANGE xTurnChange;

    if ( !chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xTurnChange) )
    {
        return ;
    }

    if ( ui->upUserLabel->text() == xTurnChange.m_szTurnNow )
    {
        // up
        QPixmap map(":/images/images/up.png");
        ui->chooselabel->setPixmap(map);
    }
    else
    {
        QPixmap map(":/images/images/down.png");
        ui->chooselabel->setPixmap(map);
    }

    if ( xTurnChange.m_szTurnNow == sendUserName_ )
    {
        m_bTurnMe = true;
    }
    else
    {
        m_bTurnMe = false;
    }

}
void playgame::whoLeave(const chess::SPacket &xPacket)
{
    chess::SSC_PUSH_USER_LEAVE xLeave;
    if(!chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xLeave))
    {
        return;
    }

    if ( ui->upUserLabel->text () == xLeave.m_szLeaveUserName )
    {
        ui->upUserLabel->clear ();
    }
    else if ( ui->downUserLabel->text () == xLeave.m_szLeaveUserName )
    {
        ui->downUserLabel->clear ();
    }
    else
    {
        return ;
    }

    QMessageBox::information(this,"info", xLeave.m_szLeaveUserName + trUtf8("已经离开！"));
}
void playgame::whoWin(const chess::SPacket &xPacket)
{
    chess::SSC_PUSH_GAME_OVER xGameover;
    if(!chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xGameover))
    {
        return;
    }
    QString qstrWinerUsername ;

    if ( m_xMyTeam == chess::EPieceTeam::None )
    {
        switch ( static_cast<chess::EPieceTeam>(xGameover.m_uWinnerTeam) )
        {
        case chess::EPieceTeam::Down:
            qstrWinerUsername = trUtf8 ("红方");
            break;
        case chess::EPieceTeam::Up:
            qstrWinerUsername = trUtf8 ("黑方");
            break;
        default :
            break;
        }
        ui->pushButton_TakeSite->show ();
    }
    else if( m_xMyTeam == static_cast<chess::EPieceTeam>( xGameover.m_uWinnerTeam) )
    {
        if ( xGameover.m_uWinReason != unsigned (chess::EWinReason::Another_Leave) )
        {
            ui->pushButton_ReGame->show ();
        }
        qstrWinerUsername = trUtf8 ("你");
    }
    else
    {
        if ( xGameover.m_uWinReason == unsigned ( chess::EWinReason::Another_Leave) )
        {
            ui->pushButton_ReGame->show ();
        }
        qstrWinerUsername = trUtf8 ("对手");
    }

    UpdateGameStatus (false);

    QMessageBox::information(this,NULL, qstrWinerUsername + trUtf8( "赢了！") );
}
void playgame::movePiece(const chess::SPacket &xPacket)
{
    chess::SSC_MOVE_PIECE_ACK xMovePieceACK;

    if ( !chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xMovePieceACK) )
    {
        return ;
    }
    switch ( static_cast<chess::EMoveErrorCode>(xMovePieceACK.m_uMoveResult) )
    {
    case chess::EMoveErrorCode::CanMove:
    {
        auto& xFrom = xMovePieceACK.m_xFrom;
        auto& xTo = xMovePieceACK.m_xTo;

        m_arrChessPanel[xTo.m_iLine][xTo.m_iRow] = m_arrChessPanel[xFrom.m_iLine][xFrom.m_iRow];
        auto& xp = m_arrChessPanel[xFrom.m_iLine][xFrom.m_iRow];

        xp.m_xTeam = chess::EPieceTeam::None;
        xp.m_xType = chess::EPieceType::None;
    }
        break;
    case chess::EMoveErrorCode::NotYouTurn:
        QMessageBox::about(this,NULL,trUtf8("现在不该你走，请等待对方行棋！"));
        break;
    case chess::EMoveErrorCode::CanMove_But_YouWillDie:
        QMessageBox::warning(this,trUtf8("警告"),trUtf8("你这样移动会被将军 !"),QMessageBox::Ok);
        break;
    case chess::EMoveErrorCode::NoMatchPieceRule:
        QMessageBox::warning(this,trUtf8("抱歉"),trUtf8("你选择的移动方式不符合规则，请重新选择移动!"),QMessageBox::Ok);
        break;
    }
    repaint();
}

void playgame::recvMessage(const chess::SPacket &xPacket)
{
    chess::SSC_PUSH_MSG xMessage;

    if ( !chess::GetStruct(xPacket.pParamBegin,xPacket.pParamEnd, xMessage) )
    {
        return ;
    }
    //将编辑的消息发送到显示框中
    ui->showText->append (
                QString(xMessage.m_szMessageFrom) + " : "
                + QString::fromUtf8 (xMessage.m_szMessageBody) );
}

void playgame::onJiangJun(const chess::SPacket &xPacket)
{
    chess::SSC_PUSH_JIANGJU xJiangJun;

    if ( !chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xJiangJun) )
    {
        return ;
    }

    if ( xJiangJun.m_szBeiJiangJuUserName == sendUserName_ )
    {
        QMessageBox::about(this, trUtf8("将军"), trUtf8("你被将军 !"));
    }
    else
    {
        QMessageBox::about(this, trUtf8("将军"), xJiangJun.m_szBeiJiangJuUserName + trUtf8(" 被将军 !"));
    }
}
void playgame::connectToServer()
{
    SetServer* server = new SetServer;
    QString    ip     = server->sendIP();
    int        port   = server->sendPort();
    qDebug() << "ip:" << ip << "port:" << port;
    if(socket_) return;
    socket_ = new QTcpSocket;
    connect(socket_, SIGNAL(connected()), this, SLOT(slot_socket_connected()));
    connect(socket_,SIGNAL(readyRead()),this,SLOT(slot_socket_readyRead()));
    connect(socket_,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(slot_socket_error(QAbstractSocket::SocketError)));
    socket_->connectToHost(ip,port);
}

QString playgame::GetImageString(const playgame::SChessPoint &xChessPoint)
{
    QString qstrImagePath ;
    switch ( xChessPoint.m_xTeam )
    {
    case chess::EPieceTeam::Down:
        qstrImagePath += "R";
        break;
    case chess::EPieceTeam::Up:
        qstrImagePath += "B";
        break;
    default :
        break;
    }

    switch (xChessPoint.m_xType)
    {
    case chess::EPieceType::Bing:
        qstrImagePath += "P";
        break;
    case chess::EPieceType::Pao:
        qstrImagePath += "C";
        break;
    case chess::EPieceType::Che:
        qstrImagePath += "R";
        break;
    case chess::EPieceType::Ma:
        qstrImagePath += "N";
        break;
    case chess::EPieceType::Xiang:
        qstrImagePath += "B";
        break;
    case chess::EPieceType::Shi:
        qstrImagePath += "A";
        break;
    case chess::EPieceType::Jiang:
        qstrImagePath += "K";
        break;
    default:
        break;
    }

    return  ":/images/images/" + qstrImagePath + ".png";
}

void playgame::UpdateGameStatus(bool bGameIsRuning)
{
    if ( bGameIsRuning)
    {
        ui->label_GameStatus->setText ( trUtf8("游戏正在运行") );
        ui->pushButton_TakeSite->hide ();
    }
    else
    {
        ui->label_GameStatus->setText ( trUtf8("游戏停止") );
    }

}
void playgame::on_sendButton_clicked()
{
    QString context = ui->sendEdit->toPlainText();
    if(context.isEmpty())
    {
        QMessageBox::warning(this,trUtf8 ("警告"),trUtf8 ("发送的内容不能为空"));
        return ;
    }
    std::string strOutput;
    chess::SCS_SEND_MSG xSendMsg;
    strncpy(xSendMsg.m_szMessageBody,context.toUtf8().data(),sizeof(xSendMsg.m_szMessageBody));
    if(chess::SerializeToString(xSendMsg, chess::CS_SEND_MSG,strOutput))
    {
        socket_->write (&strOutput[0], strOutput.size ());
        ui->sendEdit->clear();
    }
    else
    {
        qDebug() << "Some error";
    }
}
void playgame::slot_socket_readyRead()
{
//不停的对服务器发送的消息进行处理
loop :
    qDebug() << socket_->bytesAvailable() << __func__ ;
    if ( socket_->bytesAvailable () < PACKET_HEADER_SIZE ) {
        return ;
    }
    char xBuffer[ PACKET_HEADER_SIZE ];
    socket_->peek (xBuffer, PACKET_HEADER_SIZE);
    char* ptr = xBuffer;
    unsigned uPacketBytes = *(unsigned*)ptr;
    uPacketBytes = ntohl ( uPacketBytes );
    if ( socket_->bytesAvailable () < uPacketBytes || socket_->bytesAvailable () > MAX_PACKET_SIZE) {
        return ;
    }
    {
        char xBuffer[ MAX_PACKET_SIZE ];

        socket_->read (xBuffer, uPacketBytes);
        chess::SPacket xPacket;

        if ( chess::ParsePacket (xPacket, xBuffer, uPacketBytes) ) {
            qDebug() << __LINE__ << xPacket.m_uEventType ;
            switch ( xPacket.m_uEventType )
            {
            case chess::SC_LOGIN_ACK:
                judgeLogin(xPacket);
                break;
            case chess::SC_TAKESITE_ACK :
                judgeSit(xPacket);
                break;
            case chess::SC_PUSH_CHESS_PANEL :
                initGame(xPacket);
                break;
            case chess::SC_PUSH_TURN_CHANGE :
                judgeTurn(xPacket);
                break;
            case chess::SC_PUSH_MSG:
                recvMessage(xPacket);
                break;
            case chess::SC_MOVE_PIECE_ACK:
                movePiece(xPacket);
                break;
            case chess::SC_PUSH_USER_LEAVE:
                whoLeave(xPacket);
                break;
            case chess::SC_PUSH_JIANGJU:
                onJiangJun(xPacket);
                break;
            case chess::SC_PUSH_GAME_OVER:
                whoWin(xPacket);
                break;
            }
        }
    }
    if( socket_->bytesAvailable() > 0 )
    {
        goto loop;
    }
}

void playgame::slot_socket_error(QAbstractSocket::SocketError)
{
    //    qDebug() << "Some socket error !" ;
    if ( !m_bIsFirstDisconnect )
    {
        return ;
    }
    m_bIsFirstDisconnect = false;

    QMessageBox::about(this, trUtf8("与服务器连接已断开"), trUtf8("服务器断开了连接 !"));

    Login *login = new Login();

    login->show();

    deleteLater();
}

void playgame::slot_socket_connected()
{
    std::string strOutput;

    chess::SCS_LOGIN xLogin;

    //    sendUserName_ = "user2";
    qDebug() << sendUserName_;
    setWindowTitle(sendUserName_);

    strcpy(xLogin.m_szUserName, sendUserName_.toStdString().c_str());
    strcpy(xLogin.m_szPassword, sendPassword_.toStdString().c_str());
    //    strcpy(xLogin.m_szPassword, "password1");

    chess::SerializeToString(xLogin, chess::CS_LOGIN, strOutput);

    socket_->write(&strOutput[0], strOutput.size());
}

void playgame::slot_heartBeatTimeout()
{
    std::string strOutput;

    chess::SerializeToString(chess::CS_HEART_BEAT, strOutput);

    socket_->write(&strOutput[0], strOutput.size());
}

void playgame::on_pushButton_TakeSite_clicked()
{
    std::string strOutput;

    if ( chess::SerializeToString(chess::CS_TAKESITE, strOutput) )
    {
        socket_->write (&strOutput[0], strOutput.size ());
    }
}

void playgame::on_pushButton_ReGame_clicked()
{
    std::string strOutput;

    if ( chess::SerializeToString(chess::CS_REGAME, strOutput) )
    {
        socket_->write (&strOutput[0], strOutput.size ());
    }
}

