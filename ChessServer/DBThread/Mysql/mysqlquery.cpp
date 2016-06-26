#include "mysqlquery.h"

MySQLConnection::MySQLConnection()
{
    m_bIsConnected = false;
    m_MySQLConn = NULL;
    m_sHostname = "";
    m_sUsername = "";
    m_sPassword = "";
    m_wPort = 0;
    m_sSchemaName = "";
}

MySQLConnection::~MySQLConnection()
{
    if(m_MySQLConn != NULL)
    {
        std::clog << "Closing MySQL Connection" << std::endl;
        mysql_close(m_MySQLConn);
    }
}

bool MySQLConnection::Connect(const std::string &sHostname, const uint16_t &wPort, const std::string &sUsername, const std::string &sPassword, const std::string &sDB = NULL)
{
    // If we're already connected, we should close the first connection
    Disconnect();

    m_sHostname = sHostname;
    m_sUsername = sUsername;
    m_sPassword = sPassword;
    m_wPort = wPort;
    m_sSchemaName = sDB;
    m_bIsConnected = false;

    MYSQL *MySQLConnRet = NULL;
    m_MySQLConn = mysql_init(m_MySQLConn);

    my_bool bReconnect = true;

    mysql_options (m_MySQLConn, MYSQL_OPT_RECONNECT, &bReconnect);

    std::clog << "Connection to " << m_sUsername << "@" << m_sHostname << ":" << wPort << "..." ;

    MySQLConnRet = mysql_real_connect(m_MySQLConn, m_sHostname.c_str(), m_sUsername.c_str(), m_sPassword.c_str(), m_sSchemaName.c_str(), m_wPort, NULL, 0);

    if(MySQLConnRet == NULL)
    {
        m_bIsConnected = false;
        std::cerr << "Connection failed: " << mysql_error(m_MySQLConn);
    } else {
        m_bIsConnected = true;
        std::clog << "Connected!" << std::endl;
    }

    return m_bIsConnected;
}

void MySQLConnection::Disconnect()
{
    if(m_bIsConnected)
    {
        mysql_close(m_MySQLConn);
        std::clog << "Disconnected from MySQL DB!" << std::endl;
    }


    m_bIsConnected = false;

}

bool MySQLConnection::SelectDB(const std::string &sSchemaName)
{
    if(!m_bIsConnected)
    {
        std::cerr << "Not connected to MySQL DB!" << std::endl;
        return false;
    }

    if(mysql_select_db(m_MySQLConn, sSchemaName.c_str()) != 0)
    {
        std::cerr << "Failed to select DB! Error: " << mysql_error(m_MySQLConn) << std::endl;
        return false;
    } else {
        m_sSchemaName = sSchemaName.c_str();
        std::clog << "Selected database \"" << sSchemaName << "\"" << std::endl;
        return true;
    }
}

const std::string MySQLConnection::GetLastError() const
{
    if(!m_bIsConnected)
    {
        std::cerr << "Not connected to MySQL DB!" << std::endl;
        return "Not connected";
    }

    return (char*)mysql_error(m_MySQLConn);
}

int MySQLConnection::GetLastErrorNo() const
{
    return mysql_errno(m_MySQLConn);
}

bool MySQLConnection::Ping() const
{
    return 0 == mysql_ping(m_MySQLConn);
}

MYSQL *MySQLConnection::getConn()
{
    return m_MySQLConn;
}

bool MySQLConnection::IsConnected()
{
    return m_bIsConnected;
}

const std::string MySQLConnection::EscapeString(const std::string &value) const
{
    if(!m_bIsConnected)
    {
        std::cerr << "Not connected to MySQL DB!" << std::endl;
        return "";
    }

    char *cValue = new char[(value.length()*2)+1];
    mysql_real_escape_string(m_MySQLConn, cValue, value.c_str(), value.length());

    std::string sRet = cValue;
    delete [] cValue;

    return sRet;
}

void MySQLQuery::ClearMoreResult()
{
    MYSQL* pConn= m_sqlConn->getConn ();
    if ( !pConn)
    {
        return ;
    }
    while ( mysql_more_results(pConn) )
    {
        mysql_next_result(pConn);
    }
}

MySQLQuery::MySQLQuery(MySQLConnection *mConn, const std::string &sStatement)
{
    m_sqlConn = mConn;
    m_sStatement = sStatement;
    m_iResultRowCount = 0;

    int argCount = std::count(m_sStatement.begin(), m_sStatement.end(), '?');
    for(int i = 1; i <= argCount; i++)
    {
        m_mArgMap.insert(std::pair<int, std::string>(i, ""));
    }
}

MySQLQuery::~MySQLQuery()
{
    ClearMoreResult ();
}

bool MySQLQuery::setString(const unsigned int &idx, const std::string &value)
{

    if(idx > m_mArgMap.size())
    {
        std::cerr << "Index exceeds total arg count in statement" << std::endl;
        return false;
    }

    std::stringstream ss;
    std::string escapedValue = m_sqlConn->EscapeString(value);
    ss << "\"" << escapedValue << "\"";
    m_mArgMap[idx] = ss.str();

    return true;
}

bool MySQLQuery::setInt(const unsigned int &idx, const int &value)
{
    if(idx > m_mArgMap.size())
    {
        std::cerr << "Index exceeds total arg count in statement" << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << value;
    m_mArgMap[idx] = ss.str();

    return true;
}

bool MySQLQuery::setDouble(const unsigned int &idx, const double &value)
{
    if(idx > m_mArgMap.size())
    {
        std::cerr << "Index exceeds total arg count in statement" << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << value;
    m_mArgMap[idx] = ss.str();

    return true;
}

bool MySQLQuery::setTime(const unsigned int &idx, const time_t &value)
{
    if(idx > m_mArgMap.size())
    {
        std::cerr << "Index exceeds total arg count in statement" << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << value;
    m_mArgMap[idx] = ss.str();

    return true;
}

bool MySQLQuery::setNull(const unsigned int &idx)
{
    if(idx > m_mArgMap.size())
    {
        std::cerr << "Index exceeds total arg count in statement" << std::endl;
        return false;
    }

    m_mArgMap[idx] = "NULL";
    return true;
}

const std::string MySQLQuery::getFieldName(const unsigned int &field)
{
    if(field < 1)
    {
        std::cerr << "The field index has to be over 1!" << std::endl;
        return NULL;
    } else if(m_mFieldMap.size() < field){
        std::cerr << "There are only " << m_mFieldMap.size() << " fields available!" << std::endl;
        return NULL;
    }

    std::string sFieldName = m_mFieldMap[field];
    return sFieldName;
}

const std::string MySQLQuery::getString(const unsigned int &row, const unsigned int &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return NULL;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return NULL;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return NULL;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    std::string sValue = rSelectedRow[field];

    return sValue;
}

const std::string MySQLQuery::getString(const unsigned int &row, const std::string &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return "";
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return "";
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return "";
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    int iFieldID = m_mFieldStringToIntMap[field];
    std::string sValue = rSelectedRow[iFieldID];

    return sValue;
}

int MySQLQuery::getInt(const unsigned int &row, const unsigned int &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    int iValue = atoi(rSelectedRow[field].c_str());

    return iValue;
}

int MySQLQuery::getInt(const unsigned int &row, const std::string &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    int iFieldID = m_mFieldStringToIntMap[field];
    int iValue = atoi(rSelectedRow[iFieldID].c_str());

    return iValue;
}

double MySQLQuery::getDouble(const unsigned int &row, const unsigned int &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1.0;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1.0;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1.0;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    double dValue = atof(rSelectedRow[field].c_str());

    return dValue;
}

double MySQLQuery::getDouble(const unsigned int &row, const std::string &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1.0;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1.0;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1.0;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    int iFieldID = m_mFieldStringToIntMap[field];
    double dValue = atof(rSelectedRow[iFieldID].c_str());

    return dValue;
}

time_t MySQLQuery::getTime(const unsigned int &row, const unsigned int &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    time_t tValue = atoi(rSelectedRow[field].c_str());

    return tValue;
}

time_t MySQLQuery::getTime(const unsigned int &row, const std::string &field)
{
    if(GetResultRowCount() < 1)
    {
        std::cerr << "The query didn't return any rows!" << std::endl;
        return -1;
    } else if(GetResultRowCount() < row)
    {
        std::cerr << "There are only " << GetResultRowCount() << " rows available!" << std::endl;
        return -1;
    } else if(row < 1)
    {
        std::cerr << "The selected row has to be > 1" << std::endl;
        return -1;
    }

    TResultRow rSelectedRow;
    rSelectedRow = m_mResultMap[row-1];

    int iFieldID = m_mFieldStringToIntMap[field];
    time_t tValue = atoi(rSelectedRow[iFieldID].c_str());

    return tValue;
}


unsigned int MySQLQuery::GetResultRowCount()
{
    const int iRowCount = m_mResultMap.size();
    return iRowCount;
}

unsigned int MySQLQuery::GetFieldCount()
{
    const int iFieldCount = m_mFieldMap.size();
    return iFieldCount;
}

const std::string MySQLQuery::BuildQueryString()
{
    // replace each '?' with the corresponding value
    int iLastFoundPos;
    std::string sPreparedStatement;
    sPreparedStatement = m_sStatement;
    for(unsigned int i = 1; i <= m_mArgMap.size(); i++)
    {
        iLastFoundPos = sPreparedStatement.find( '?' );
        sPreparedStatement.replace(iLastFoundPos, 1, "");
        sPreparedStatement.insert(iLastFoundPos, m_mArgMap[i]);
    }

    return sPreparedStatement;
}

bool MySQLQuery::ExecuteQuery()
{
    std::string sStatement = BuildQueryString();

    if(mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
    {
        std::cerr << "MySQL Error: " << m_sqlConn->GetLastError() << std::endl;
        return false;
    }

    MYSQL_RES *result = mysql_store_result(m_sqlConn->getConn());

    if(result == NULL)
    {
        std::cerr << "MySQL Error: " << m_sqlConn->GetLastError() << std::endl;
        return false;
    }


    int iNumFields = mysql_num_fields(result);
    MYSQL_ROW row;
    MYSQL_FIELD *field;

    // Get field names and store it in the map
    int i = 0;
    while((field = mysql_fetch_field(result)))
    {
        m_mFieldMap.insert(std::pair<int, std::string>(i, field->name));
        m_mFieldStringToIntMap.insert(std::pair<std::string, int>(field->name, i));
        i++;
    }

    // Get Rows
    i = 0;
    while((row = mysql_fetch_row(result)))
    {
        TResultRow resRow;
        for(int n = 0; n < iNumFields; n++)
        {
            resRow.insert(std::pair<int, std::string>(n, row[n] ? row[n] : "NULL"));
        }

        m_mResultMap.insert(std::pair<int, TResultRow>(i, resRow));

        i++;
    }

    return true;
}

bool MySQLQuery::ExecuteUpdate()
{
    std::string sStatement = BuildQueryString();

    if(mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
    {
        std::cerr << "MySQL Error: " << m_sqlConn->GetLastError() << std::endl;
        return false;
    }

    return true;
}

int MySQLQuery::ExecuteInsert()
{
    std::string sStatement = BuildQueryString();

    if(mysql_query(m_sqlConn->getConn(), sStatement.c_str()))
    {
        std::cerr << "MySQL Error: " << m_sqlConn->GetLastError() << std::endl;
        return 0;
    }

    int iLastInsertID = mysql_insert_id(m_sqlConn->getConn());

    return iLastInsertID;
}

