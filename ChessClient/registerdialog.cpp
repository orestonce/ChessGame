#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "setserverdialog.h"
#include "database.h"
#include "CSProtocol.h"
#include <QMessageBox>
#include <QWebSocket>
#include <QUrl>
#include <QJsonDocument>
#include <QTimer>
#include "rpchub.h"

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    QString style = "border:2px groove gray;border-radius:4px;";
    ui->lineEdit_userName->setStyleSheet(style);
    ui->lineEdit_password1->setStyleSheet(style);
    ui->lineEdit_password2->setStyleSheet(style);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_pushButton_ok_clicked()
{
    QString userName = this->ui->lineEdit_userName->text();
    QString password1 = this->ui->lineEdit_password1->text();
    QString password2 = this->ui->lineEdit_password2->text();

    if(userName.isEmpty())
    {
        QMessageBox::information(this,"警告","用户名不能为空，请重新输入!",QMessageBox::Ok);
        return ;
    }
    else if(password1.isEmpty())
    {
        QMessageBox::information(this,"警告","密码不能为空，请重新输入!",QMessageBox::Ok);
        return ;
    }
    else if(password1 != password2)
    {
        QMessageBox::information(this,"警告","密码不匹配!",QMessageBox::Ok);
        return ;
    }

    QString lastError;
    RpcHub hub;
    bool hasError = !hub.InitWebsocket(Database::GetInstance()->GetServerAddr(), 10);
    RegisterRequest req;
    RegisterResponse resp;
    req.Username = userName;
    req.Password = password1;
    if (hasError || !hub.SendAndRecv(req, resp) || !resp.ErrMsg.isEmpty())
    {
        lastError = resp.ErrMsg;
        hasError = true;
    }
    if (hasError) {
        if(lastError.isEmpty()) {
            lastError = "未知错误.";
        }
        if (this->isVisible()){
            QMessageBox::information(this, "失败", lastError);
        }
    } else {
        if (this->isVisible()){
            QMessageBox::information(this, "成功", "注册成功");
        }
        this->close();
    }
}

void RegisterDialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

