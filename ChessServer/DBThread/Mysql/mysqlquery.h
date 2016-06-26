#ifndef MYSQLQUERY_H
#define MYSQLQUERY_H

#include <mysql/mysql.h>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <map>
#include <iostream>
#include <sstream>

class MySQLConnection
{

public:
    MySQLConnection();
    ~MySQLConnection();
    // connects to a MySQL-server
    bool Connect(const std::string &sHostname, const uint16_t &wPort, const std::string &sUsername, const std::string &sPassword, const std::string &sDB);
    // selects a DB
    bool SelectDB(const std::string &sSchemaName);
    // disconnects (huh, who guessed this?)
    void Disconnect();
    // returns the last error string
    const std::string GetLastError() const;
    // 最后一次的错误号
    int GetLastErrorNo() const;
    // 在最开始链接的时候
    bool Ping() const;
    // gets a pointer to the MySQL connection
    MYSQL *getConn();
    // returns true when connected
    bool IsConnected();
    // returns an escaped string
    const std::string EscapeString(const std::string &value) const;

private:
    MYSQL *m_MySQLConn;
    bool m_bIsConnected;
    std::string m_sHostname;
    std::string m_sUsername;
    std::string m_sPassword;
    std::string m_sSchemaName;
    uint16_t m_wPort;
};

class MySQLQuery
{
public:

    MySQLQuery(MySQLConnection *mConn, const std::string &sStatement);
    ~MySQLQuery();

    // sets the value of idx to a given string (also adds quotation marks and escapes the string)
    bool setString(const unsigned int &idx, const std::string &value);
    // sets the value of idx to a given int
    bool setInt(const unsigned int &idx, const int &value);
    // sets the value of idx to a given double
    bool setDouble(const unsigned int &idx, const double &value);
    // sets the value of idx to a given time_t
    bool setTime(const unsigned int &idx, const time_t &value);
    // sets the value of idx to NULL
    bool setNull(const unsigned int &idx);

    // executes a SELECT-statement
    bool ExecuteQuery();
    // executes an UPDATE-statement
    bool ExecuteUpdate();
    // executes an INSERT-statement and returns the last inserted ID
    int ExecuteInsert();

    // builds the query string with filled-in arguments and returns it
    const std::string BuildQueryString();

    // returns a field name
    const std::string getFieldName(const unsigned int &field);
    // gets a string value from the given row and field
    const std::string getString(const unsigned int &row, const unsigned int &field);
    const std::string getString(const unsigned int &row, const std::string &field);
    // gets an int value from the given row and field
    int getInt(const unsigned int &row, const unsigned int &field);
    int getInt(const unsigned int &row, const std::string &field);
    // gets a double value from the given row and field
    double getDouble(const unsigned int &row, const unsigned int &field);
    double getDouble(const unsigned int &row, const std::string &field);
    // gets a time value from the given row and field
    time_t getTime(const unsigned int &row, const unsigned int &field);
    time_t getTime(const unsigned int &row, const std::string &field);

    // returns the result row count
    unsigned int GetResultRowCount();
    unsigned int GetFieldCount();

    /**
     * @brief ClearMoreResult 清除后来的结果
     */
    void ClearMoreResult();

private:
    typedef std::map<int, std::string> TResultRow;
    MySQLConnection *m_sqlConn;
    int m_iResultRowCount;
    std::string m_sStatement;
    std::map<int, std::string> m_mArgMap;
    std::map<int, TResultRow> m_mResultMap;
    std::map<int, std::string> m_mFieldMap;
    std::map<std::string, int> m_mFieldStringToIntMap;

};

#endif // MYSQLQUERY_H
