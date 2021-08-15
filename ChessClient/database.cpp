#include "database.h"
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

static const char *k2ServerAddress = "k2ServerAddress";
static const char *k2Username = "k2Username";
static const char *k2Password = "k2Password";
static const char *k2RoomId = "k2RoomId";
static const char *k2RememberPassword = "k2RememberPassword";
static const char *default_address = "ws://49.233.210.51:8912/ChessGame";

Database *Database::GetInstance()
{
    static Database gDb;
    return &gDb;
}

Database::Database()
{
    this->db = new QSqlDatabase();
    *this->db = QSqlDatabase::addDatabase("QSQLITE");
    this->db->setDatabaseName("ChineseChess.db");
    if (!db->open()) {
        QMessageBox::information(NULL, "错误", "无法创建本地数据库 !", QMessageBox::Ok);
        exit(-1);
    }
    QSqlQuery query(*db);
    query.prepare("create table kvTable(key char(255) primary key, value varchar(65535));");
    query.exec();
}

QString Database::GetServerAddr()
{
    QString addr = this->getValue(k2ServerAddress);
    if (addr.isEmpty())
    {
        addr = default_address;
    }
    return addr;
}

void Database::SetServerAddr(QString addr)
{
    this->setValue(k2ServerAddress, addr);
}

QString Database::GetDefaultServerAddr()
{
    return default_address;
}

QString Database::GetUsername()
{
    return this->getValue(k2Username);
}

QString Database::GetPassword()
{
    return this->getValue(k2Password);
}

QString Database::GetRoomId()
{
    return this->getValue(k2RoomId);
}

void Database::SaveData(QString username, QString password, QString roomId)
{
    this->setValue(k2Username, username);
    this->setValue(k2Password, password);
    this->setValue(k2RoomId, roomId);
}

void Database::CleanData()
{
    this->SaveData("", "", "");
}

void Database::SetRememberPassword(bool remeber)
{
    this->setValue(k2RememberPassword, remeber ? "true" : "false");
}

bool Database::IsRememberPassword()
{
    return this->getValue(k2RememberPassword) == "true";
}

QString Database::getValue(QString key)
{
    QSqlQuery query(*db);
    query.prepare("SELECT value FROM kvTable WHERE key = :key ; ");
    query.bindValue(":key", key);
    query.exec();

    QString result;
    if (query.next()) {
        result = query.value(0).toString();
    }
    return result;
}

void Database::setValue(QString key, QString value)
{
    QSqlQuery query(*db);
    query.prepare("REPLACE INTO kvTable VALUES(:key, :value);");

    query.bindValue(":key", key);
    query.bindValue(":value", value);
    query.exec();
}
