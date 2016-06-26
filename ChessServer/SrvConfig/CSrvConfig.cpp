#include "CSrvConfig.h"
#include "RoLog/RoLog.h"
#include "inifile/inifile.h"

#define CHECK_RET( ret ) if ( ret != 0 ) \
{\
START_LOG(RO_CERR).OUT_LOG("Get value error !").END_LOG();\
return false;\
}

namespace chess
{

bool CSrvConfig::Init(const char *szFileName)
{
    inifile::IniFile file;

    if ( 0 != file.load (szFileName) )
    {
        START_LOG(RO_CERR).OUT_LOG("Open file error : ").BUILD_LOG( szFileName ).END_LOG();

        return false;
    }

    std::string strSection = "ChessServer";
    int ret ;

    m_uMaxUserCount =  file.getIntValue (strSection, "MaxUserCount", ret );
    CHECK_RET(ret);

    m_uListenPort = file.getIntValue (strSection, "ListenPort", ret);
    CHECK_RET(ret);

    m_uRecvBufferByte = file.getIntValue (strSection, "RecvBufferByte", ret);
    CHECK_RET( ret );

    m_iHeartBeatOverTime = file.getIntValue (strSection, "HeartBeatOverTime", ret);
    CHECK_RET(ret);

    strSection = "MySQL";

    m_uDBThreadCount = file.getIntValue (strSection, "ThreadCount", ret);
    CHECK_RET( ret );

    m_xMySQLConn.m_strDatabase = file.getStringValue (strSection, "Database", ret);
    CHECK_RET( ret );

    m_xMySQLConn.m_strHost = file.getStringValue (strSection, "Host", ret);
    CHECK_RET( ret);

    m_xMySQLConn.m_uPort = file.getIntValue (strSection, "Port", ret);
    CHECK_RET( ret);

    m_xMySQLConn.m_strUser = file.getStringValue (strSection, "User", ret);
    CHECK_RET ( ret );

    m_xMySQLConn.m_strPassword = file.getStringValue (strSection, "Password", ret);
    CHECK_RET( ret );

    return true;
}

}
