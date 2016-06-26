#ifndef CSRVCONFIG_H
#define CSRVCONFIG_H

#include "RoSpace/RoSingleton.h"
#include "RoSpace/RoTypes.h"
#include <string>

namespace chess
{
using namespace RoSpace;

class CSrvConfig
        : public RoSpace::CSingleton<CSrvConfig>
{
public:
    struct _SMySQLConn
    {
        std::string m_strHost; // 数据库主机
        USHORT   m_uPort;    // 数据库端口号
        std::string m_strUser;  // 数据库用户名
        std::string m_strPassword; // 数据库密码
        std::string m_strDatabase;  // 数据库名
    };

    bool Init(const char* szFileName);

    UINT       GetMaxUserCount() const { return m_uMaxUserCount; }
    USHORT GetListenPort() const { return m_uListenPort; }
    UINT       GetRecvBufferByte() const { return m_uRecvBufferByte; }
    UINT       GetDBThreadCount() const { return m_uDBThreadCount; }
    _SMySQLConn const& GetMySQLConn() const { return m_xMySQLConn; }
    int       GetHeartBeatOverTime() const { return m_iHeartBeatOverTime; }
private:
    UINT        m_uMaxUserCount;  // 最大用户数目
    USHORT  m_uListenPort;          // 监听端口
    UINT        m_uRecvBufferByte; // 接收缓冲区大小
    int            m_iHeartBeatOverTime; // 心跳包超时秒数
    UINT        m_uDBThreadCount;// 数据库线程数目
    _SMySQLConn m_xMySQLConn; // 数据库链接信息
};
#define g_pSrvConfig chess::CSrvConfig::Instance ()

}
#endif // CSRVCONFIG_H
