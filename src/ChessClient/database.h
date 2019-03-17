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
    void ResetDefaultServerAddr();
    QString GetUsername();
    QString GetPassword();
    void SetUsernameAndPassword(QString username, QString password);
    void CleanUsernameAndPassword();
    void SetRememberPassword(bool remeber);
    bool IsRememberPassword();
private:
    QString getValue(QString key );
    void setValue(QString key ,QString value );
private :
    QSqlDatabase *db;
};

#endif // DATABASE_H
