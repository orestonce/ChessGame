#include "login.h"
#include "gamepanel.h"
#include "registerdialog.h"
#include "ui_login.h"
#include "setserverdialog.h"
#include "database.h"
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QTcpSocket>
#include <QtWebSockets/QtWebSockets>


// 把窗口放到最顶层
template <typename T>
void show_top_level(T *obj)
{
    obj->show();
    obj->activateWindow();
    obj->raise();
}

void swap_dialog(Login* login,QDialog *dlg)
{
    login->hide();
    show_top_level(dlg);
    QObject::connect(dlg,SIGNAL(finished(int)), login, SLOT(show()));
    dlg->exec();
    dlg->close();
    delete dlg;
    show_top_level(login);
}

Login::Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Login)
{
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    QString style = "border:2px groove gray;border-radius:4px;";
    ui->passwordEdit->setStyleSheet(style);
    ui->userEdit->setStyleSheet(style);
    ui->roomEdit->setStyleSheet(style);

    this->loadStatusFromDB();
    connect(ui->rememberpasswordBox, &QCheckBox::stateChanged, this, &Login::saveStatusToDB);
}

Login::~Login()
{
    delete ui;
}


void Login::on_loginButton_clicked()
{
    QString username = ui->userEdit->text();
    QString password = ui->passwordEdit->text();
    QString roomId = ui->roomEdit->text();

    if(username.isEmpty())
    {
        QMessageBox::information(this,"警告","用户名不能为空，请重新输入！",QMessageBox::Ok);
        return ;
    } else if(password.isEmpty()) {
        QMessageBox::information(this,"警告","密码不能为空，请重新输入！",QMessageBox::Ok);
        return ;
    } else if(roomId.isEmpty()) {
        QMessageBox::information(this,"警告","房间不能为空，请重新输入！",QMessageBox::Ok);
        return ;
    }
    // 保存一下状态.
    this->saveStatusToDB();

    GamePanel * play = new GamePanel;
    connect(play,SIGNAL(finished(int)), this, SLOT(show()));
    this->hide();
    show_top_level(play);
    if (play->Init(username, password, roomId)) {
        play->exec();
    }
    play->close();
    delete play;
    show_top_level(this);
}
void Login::on_registerButton_clicked()
{
    RegisterDialog* reg = new RegisterDialog;
    swap_dialog(this, reg);
}

void Login::on_setServerButton_clicked()
{
    SetServerDialog *dlg = new SetServerDialog;
    swap_dialog(this, dlg);
}

void Login::on_cancelButton_clicked()
{
    close();
}

void Login::loadStatusFromDB()
{
    bool isRemember = Database::GetInstance()->IsRememberPassword();
    ui->rememberpasswordBox->setChecked( isRemember );
    if( isRemember )
    {
        ui->userEdit->setText(Database::GetInstance()->GetUsername());
        ui->passwordEdit->setText(Database::GetInstance()->GetPassword());
        ui->roomEdit->setText(Database::GetInstance()->GetRoomId());
    }
}

void Login::saveStatusToDB()
{
    bool isRemember = ui->rememberpasswordBox->isChecked();
    Database::GetInstance()->SetRememberPassword(isRemember);
    if (isRemember) {
        Database::GetInstance()->SaveData(ui->userEdit->text(),ui->passwordEdit->text(),ui->roomEdit->text());
    } else {
        Database::GetInstance()->CleanData();
    }
}
