#ifndef RONETTYPES_H
#define RONETTYPES_H

#include "RoSpace/RoTypes.h"
#include <cstring>

namespace RoNet
{

/**
 * RoNet 网络库
 *
 */
using namespace RoSpace;

// 应用逻辑
typedef void (*FP_AppLogic)();
/**
 * 接收链接成功
 *  参数 : uConnIndex [IN] 新到来的链接的索引 [1, uMaxConnCount)
 */
typedef void (*FP_OnAcceptOK) (UINT uConnIndex);
/**
 * 链接被断开
 *  参数 : uConnIndex [IN] 被断开的链接的索引 [1, uMaxConnCount)
 */
typedef void (*FP_OnDisconnected) (UINT uConnIndex);

/**
 *接收数据成功
 *  参数 : uConnIndex [IN] 接收到数据的链接索引 [1,uMaxConnCount)
 *  pBuffer [IN] 数据包
 *  uBufferBytes [IN] 数据包大小
 */
typedef void (*FP_OnRecvData)(UINT uConnIndex, const char* pBuffer, UINT uBufferBytes);

struct SRoNetInitData
{
    // 各种回调函数
    FP_AppLogic       m_fpAppLogic;  // 应用逻辑
    FP_OnAcceptOK m_fpOnAcceptOK; // 接收成功
    FP_OnDisconnected m_fpOnDisconnected; // 断开链接
    FP_OnRecvData        m_fpOnRecvData; // 收到数据

    USHORT m_uListenPort; // 监听端口
    UINT m_uMaxConnCount; // 最大链接数
    UINT m_uRecvBufferByte;// 接收缓冲大小
public :
    SRoNetInitData()
    {
        memset(this, 0, sizeof(*this));
    }
};

}

#endif // RONETTYPES_H
