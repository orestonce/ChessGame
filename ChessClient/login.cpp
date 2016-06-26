#include "login.h"
#include "playgame.h"
#include "register.h"
#include "setserver.h"
#include "ui_login.h"
#include "Protol/CSProtol.h"
#include "Protol/Common.h"
#include "Protol/CSProtol.Parser.hpp"
#include "Protol/CSProtol.Serialize.hpp"
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QTcpSocket>
#include <QDebug>

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    //绘制背景图片
    this->setStyleSheet("QMainWindow {border-image:url(:/images/images/back3.jpg)}");

    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->passwordEdit->setStyleSheet("lineedit-password-character: 42;border:2px groove gray;border-radius:4px;");
    ui->userEdit->setStyleSheet("border:2px groove gray;border-radius:4px;");
//    ui->cancelButton->setStyleSheet("border:3px groove gray;border-radius:5px;");
    readingBoxStatus();
    remember_password_status_ = ui->rememberpasswordBox->isChecked();
    if(true == remember_password_status_)
    {
        readingUsername();
        readngPassword();
    }
}

Login::~Login()
{
    delete ui;
    server->close();
}

void Login::rememberUsername()
{
    username_ = ui->userEdit->text();
    QFile file("username.dat");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&file);
    out << username_;
    file.close();
}

void Login::rememberPassword()
{
    password_ = ui->passwordEdit->text();
    QFile file("password.dat");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&file);
    out << password_;
    file.close();
}

void Login::rememberBoxStatus()
{
    remember_password_status_ = ui->rememberpasswordBox->isChecked();
    QFile file("status.dat");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&file);
    out << remember_password_status_;
    file.close();
}
void Login::readingUsername() {
    QString username;
    QFile file("username.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in >> username;
    ui->userEdit->setText(username);
    file.close();
}

void Login::readngPassword()
{
    QString password;
    QFile file("password.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in >> password;
    ui->passwordEdit->setText(password);
    file.close();
}

void Login::readingBoxStatus()
{
    bool checkBoxStatus;
    QFile file("status.dat");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    in >> checkBoxStatus;
    ui->rememberpasswordBox->setChecked(checkBoxStatus);
    file.close();
}
//void Login::judgeLoginSuccess(const chess::SSC_LOGIN_ACK &login) {
//    switch (login.m_uLoginResult) {
//    case chess::SSC_LOGIN_ACK::LOGIN_OK :
//        {
//        QMessageBox::information(this,NULL,trUtf8("登录成功！！"),QMessageBox::Ok);
//        playgame * play = new playgame;

//        disconnect(socket_, SIGNAL(readyRead()),
//                   this, SLOT(slot_socket_readyRead()) );
//        play->setSocket(socket_);
//        play->SetCurrentUserName(ui->userEdit->text());
//        play->show();
//        close();
//    }
//        break;
//    default:
//        QMessageBox::information(this,NULL,trUtf8("登录失败！！"),QMessageBox::Ok);
//        break;
//    }

//}
void Login::on_loginButton_clicked()
{
    username_ = ui->userEdit->text();
    password_ = ui->passwordEdit->text();
    remember_password_status_ = ui->rememberpasswordBox->isChecked();

    if(username_.isEmpty())
    {
        QMessageBox::information(this,trUtf8("警告"),trUtf8("用户名不能为空，请重新输入！！"),QMessageBox::Ok);
        return ;
    }
    else if(password_.isEmpty()) {
        QMessageBox::information(this,trUtf8("警告"),trUtf8("密码不能为空，请重新输入！！"),QMessageBox::Ok);
        return ;
    }

    rememberBoxStatus();

    if(true == remember_password_status_)
    {
        rememberUsername();
        rememberPassword();
    }

    playgame * play = new playgame;
    play->setUserInfo(username_,password_);
    play->show();
    close();
}
void Login::on_registerButton_clicked()
{
    Register* reg = new Register;
    reg->show();
    close();
}
void Login::on_setServerButton_clicked()
{
    server->show();
    close();
}

void Login::on_cancelButton_clicked()
{
    close();
}
