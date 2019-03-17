#include "setserverdialog.h"
#include "ui_setserverdialog.h"
#include <QPainter>
#include <QFile>
#include "database.h"

SetServerDialog::SetServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetServerDialog)
{
    ui->setupUi(this);
    this->loadServerAddress();
    connect(this, &QDialog::accepted, [&](){
        this->saveServerAddress();
    });
}

SetServerDialog::~SetServerDialog()
{
    delete ui;
}

void SetServerDialog::on_pushButton_ok_clicked()
{
    this->accept();
}

void SetServerDialog::on_pushButton_cancel_clicked()
{
    this->reject();
}

void SetServerDialog::loadServerAddress()
{
    QString addr = Database::GetInstance()->GetServerAddr();
    ui->lineEdit_serverAddr->setText(addr);
}

void SetServerDialog::saveServerAddress()
{
    QString addr =  ui->lineEdit_serverAddr->text();
    Database::GetInstance()->SetServerAddr(addr);
}

void SetServerDialog::on_pushButton_reset_clicked()
{
    Database::GetInstance()->ResetDefaultServerAddr();
    this->loadServerAddress();
}
