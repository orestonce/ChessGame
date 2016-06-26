#include "register.h"
#include "ui_register.h"
#include "login.h"
#include "setserver.h"
#include "Protol/CSProtol.h"
#include "Protol/Common.h"
#include "Protol/CSProtol.Parser.hpp"
#include "Protol/CSProtol.Serialize.hpp"
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QTcpSocket>

Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode (QLineEdit::Password);
    ui->confirmEdit->setEchoMode(QLineEdit::Password);
    ui->userEdit->setStyleSheet("border:2px groove gray;border-radius:4px");
    ui->passwordEdit->setStyleSheet("lineedit-password-character: 42;border:2px groove gray;border-radius:4px;");
    ui->confirmEdit->setStyleSheet("lineedit-password-character: 42;border:2px groove gray;border-radius:4px;");
    ui->confirmButton->setStyleSheet("border:2px groove gray;border-radius:5px;");
    ui->canelButton->setStyleSheet("border:2px groove gray;border-radius:5px;");
    connectToServer();
}

Register::~Register()
{
    delete ui;
}

void Register::connectToServer()
{
    SetServer* server = new SetServer;
    QString ip = server->sendIP();
    int port   = server->sendPort();
    if(socket_) return;
    socket_ = new QTcpSocket(this);
    qDebug() << "ip:" << ip << "port:" << port;
    connect(socket_,SIGNAL(readyRead()),this,SLOT(on_socket_readyRead()));
    connect(socket_,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socket_error(QAbstractSocket::SocketError)));
    socket_->connectToHost(ip,port);
    socketFlag = true;
}

void Register::on_confirmButton_clicked()
{
    password_ = ui->passwordEdit->text();
    confirm_password_ = ui->confirmEdit->text();
    username_ = ui->userEdit->text();
    if(username_.isEmpty())
    {
        QMessageBox::information(this,trUtf8("警告"),trUtf8("用户名不能为空，请重新输入"),QMessageBox::Ok);
    }
    else if(password_.isEmpty())
    {
        QMessageBox::information(this,trUtf8("警告"),trUtf8("密码不能为空，请重新输入"),QMessageBox::Ok);
    }
    else if(confirm_password_.isEmpty())
    {
        QMessageBox::information(this,trUtf8("警告"),trUtf8("密码不一致，请重新输入"),QMessageBox::Ok);
    }
    else if(!password_.isEmpty() && !confirm_password_.isEmpty() && password_ != confirm_password_)
    {
        QMessageBox messagebox(QMessageBox::Warning,trUtf8("警告"),trUtf8 ("两次输入密码不一致密码，请重新输入"),QMessageBox::Ok);
        if(messagebox.exec() == QMessageBox::Ok) {
            ui->passwordEdit->clear();
            ui->confirmEdit->clear();
        }
    }
    else if(!password_.isEmpty() && !username_.isEmpty() && password_ == confirm_password_) {
        sendUserInfo();
    }
}

void Register::sendUserInfo()
{
    chess::SCS_REGISTER xRegister;
    strncpy(xRegister.m_szUserName,ui->userEdit->text().toStdString().c_str(),sizeof(xRegister.m_szUserName));
    strncpy(xRegister.m_szPassword,ui->passwordEdit->text().toStdString().c_str(),sizeof(xRegister.m_szPassword));
    std::string strOutput;

    if(chess::SerializeToString(xRegister,chess::CS_REGISTER,strOutput))
    {
        socket_->write(&strOutput[0],strOutput.size());
    }else
    {
        qDebug() << "SeizlizeToString error  !";
    }
}
void Register::on_socket_readyRead()
{
    if(socket_->bytesAvailable() < PACKET_HEADER_SIZE)
    {
        return ;
    }
    char xBuffer[ PACKET_HEADER_SIZE ];
    socket_->peek(xBuffer,PACKET_HEADER_SIZE);
    char* ptr = xBuffer;
    unsigned uPacketBytes = *(unsigned*)ptr;
    uPacketBytes = ntohl (uPacketBytes);
    if(socket_->bytesAvailable() < uPacketBytes || socket_->bytesAvailable() > MAX_PACKET_SIZE)
    {
        return ;
    }
    {
        char xBuffer [MAX_PACKET_SIZE];
        socket_->read(xBuffer, uPacketBytes);
        chess::SPacket xPacket;

        if(chess::ParsePacket(xPacket,xBuffer,uPacketBytes))
        {
            if(xPacket.m_uEventType == chess::SC_REGISTER_ACK)
            {
                chess::SSC_REGISTER_ACK xRegisterACK;
                if(!chess::GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xRegisterACK))
                {
                    return;
                }
                switch (xRegisterACK.m_uRegisterResult)
                {
                case chess::SSC_REGISTER_ACK::REGISTER_OK:
                {
                    QMessageBox::information(this,"Title",trUtf8("注册成功!!"),QMessageBox::Ok);
                    //注册成功后断开连接，进入登录界面
                    socket_->disconnectFromHost();
                    socket_->waitForDisconnected();
                    Login* login = new Login;
                    login->show();
                    close();
                }
                    break;
                case chess::SSC_REGISTER_ACK::USERNAME_EXISTS:
                    QMessageBox::information(this,"出错",trUtf8("用户名已经存在，请输入别的用户名，请重新注册!!"),QMessageBox::Ok);
                    break;
                default:
                    QMessageBox::information(this,"出错",trUtf8("注册失败，请重新注册!!"),QMessageBox::Ok);
                    break;
                }
            }
//            else if(xPacket.m_uEventType == chess::SC_LOGIN_ACK) {
//                chess::SSC_LOGIN_ACK xLoginACK;
//                if( chess::GetStruct(xPacket.pParamBegin,xPacket.pParamEnd,xLoginACK)) {
//                    QMessageBox::about(this,"LoginACK" ,
//                                       QString(xLoginACK.m_szUserName) +
//                                       std::to_string( xLoginACK.m_uLoginResult).c_str() );
//                }
//            }
//            else if(xPacket.m_uEventType == chess::SC_PUSH_MSG) {
//            }
        }
    }
}
void Register::on_socket_error(QAbstractSocket::SocketError) {
    qDebug() << "Some socket error !" ;
}

void Register::paintEvent(QPaintEvent *)
{
    QPixmap pixmap = QPixmap(":/images/images/back3.jpg").scaled(this->size());
    QPainter painterBackground(this);
    painterBackground.drawPixmap(this->rect(),pixmap);
}

void Register::on_canelButton_clicked()
{
//    if(socketFlag == true)
//    {
//        socket_->disconnectFromHost();
//        socket_->waitForDisconnected();
//        socketFlag = false;
//    }
    Login* login = new Login;
    login->show();
    close();
}
