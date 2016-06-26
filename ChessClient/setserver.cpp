#include "setserver.h"
#include "ui_setserver.h"
#include "login.h"
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QFile>
SetServer::SetServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetServer)
{
    ui->setupUi(this);

//    this->setStyleSheet ("border:2px groove gray;border-radius:5px;");
    ui->cancelButton->setStyleSheet("border:2px groove gray;border-radius:5px;");
    ui->confirmButton->setStyleSheet("border:2px groove gray;border-radius:5px;");
    ui->recoveryButton->setStyleSheet("border:2px groove gray;border-radius:5px;");
    ui->ipEdit->setStyleSheet("border:2px groove gray;border-radius:4px;");
    ui->portEdit->setStyleSheet("border:2px groove gray;border-radius:4px;");
    readingAddress();
    readingPort();
    ui->ipEdit->setText ( address_ );
    ui->portEdit->setText (port_);
}

SetServer::~SetServer()
{
    delete ui;
}

void SetServer::rememberAddress()
{
    address_ =  ui->ipEdit->text();
    QFile address_file("address.dat");
    address_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&address_file);
    out << address_;
    address_file.close();
}

void SetServer::rememberPort()
{
    port_ =  ui->portEdit->text();
    QFile port_file("port.dat");
    port_file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out(&port_file);
    out << port_;
    port_file.close();
}

void SetServer::readingAddress()
{
    QFile file("address.dat");
    file.open(QIODevice::ReadOnly);
    if ( !file.isOpen () )
    {
        address_ = default_address;
        return ;
    }
    QDataStream in(&file);
    in >> address_;
    ui->ipEdit->setText(address_);
    file.close();
}

void SetServer::readingPort()
{
    QFile file("port.dat");
    file.open(QIODevice::ReadOnly);
    if ( !file.isOpen () )
    {
        port_ = default_port;
        return ;
    }

    QDataStream in(&file);
    in >> port_;
    ui->portEdit->setText(port_);
    file.close();
}

QString SetServer::sendIP()
{
    return address_;
}

int SetServer::sendPort()
{

    return port_.toInt ();
}

void SetServer::on_recoveryButton_clicked()
{
    ui->ipEdit->setText(default_address);
    ui->portEdit->setText(default_port);
    rememberAddress();
    rememberPort();
}

void SetServer::on_confirmButton_clicked()
{
    if(ui->ipEdit->text().isEmpty())
    {
        QMessageBox::information(this,trUtf8("警告"),"IP地址不能为空，请输入！",QMessageBox::Ok);
    }
    else if(!ui->ipEdit->text().isEmpty())
    {
        if(ui->portEdit->text().isEmpty())
        {
            QMessageBox::information(this,trUtf8("警告"),"端口不能为空，请输入！",QMessageBox::Ok);
        }
        else if(!ui->portEdit->text().isEmpty())
        {
            rememberAddress();
            rememberPort();
            Login* login = new Login;
            login->show();
            close();
        }
    }
}

void SetServer::on_cancelButton_clicked()
{
    Login* login = new Login();
    login->show();
    this->close();
}

void SetServer::paintEvent(QPaintEvent *)
{
    QPixmap pixmap = QPixmap(":/images/images/back6.jpg").scaled(this->size());
    QPainter painterBackground(this);
    painterBackground.drawPixmap(this->rect(),pixmap);

}



