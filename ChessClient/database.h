#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
class QSqlDatabase;

class Database
{
public:
    static Database* GetInstance();
    Database();
    QString GetServerAddr();
    void SetServerAddr(QString addr);
    QString GetDefaultServerAddr();
    QString GetUsername();
    QString GetPassword();
    QString GetRoomId();
    void SaveData(QString username, QString password, QString roomId);
    void CleanData();
    void SetRememberPassword(bool remeber);
    bool IsRememberPassword();
private:
    QString getValue(QString key );
    void setValue(QString key ,QString value );
private :
    QSqlDatabase *db;
};

#endif // DATABASE_H
