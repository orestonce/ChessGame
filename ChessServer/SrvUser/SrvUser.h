#ifndef SRVUSER_H
#define SRVUSER_H

#include "RoSpace/RoTypes.h"
#include "EventBuffer/EventBuffer.h"
#include "EventBuffer/EventBufferRecycle.h"
#include "DBThread/DBThreadMgr.h"

namespace chess
{
struct SEventBuffer;
using namespace RoSpace;
struct SPacket;

/**
 * @brief The CSrvUser class 服务端用户, 代表一个与远程计算机建立的链接
 */
class CSrvUser
{
public:
    CSrvUser();
    /**
     * @brief SetConnIndex 设置链接索引, 便于自己通知用户管理器自己的改变 [只在初始化用户管理器时使用]
     * @param uConnIndex 链接索引
     */
    void SetConnIndex(UINT uConnIndex);
    /**
     * @brief GetConnIndex 获取链接索引
     * @return 自己的链接索引
     */
    UINT GetConnIndex() const;
    /**
     * @brief IsValid 判断用户是否有效[ 有效用户就是建立了链接的用户 ]
     * @return true -> 用户有效, false -> 用户无效
     */
    bool IsValid() const;

    /**
     * @brief IsLogined 判断用户是否已经登陆
     * @return true -> 已登陆, false -> 未登陆
     */
    bool IsLogined() const;
    /**
     * @brief Reset 重置用户状态
     */
    void Reset();
    /**
     * @brief SetValid 设置该用户为有效用户
     */
    void SetValid();

    /**
     * @brief OnRecvData 该用户收到远程计算机发来的数据包后的处理
     * @param pBuffer 数据包
     * @param uBufferBytes 数据包大小
     */
    void OnRecvData(const char* pBuffer, UINT uBufferBytes);
    /**
     * @brief OnAcceptOK 该用户被以接收成功的方式创建时候的处理函数
     */
    void OnAcceptOK();
    /**
     * @brief ReverseDelete 预约删除该用户. 注意, 断开有效用户链接请务必使用该方法
     */
    void ReverseDelete();

    /**
     * @brief ProcessEvent 处理用户事件, [来自数据库]
     * @param pEventBuffer 事件缓冲
     */
    void ProcessEvent(SEventBuffer* pEventBuffer);

    /**
     * @brief GetUserName 获取用户名
     * @return 用户名
     */
    std::string const& GetUserName() const
    {
        return m_strUserName;
    }

    /**
     * @brief CheckHeartBeat 检查用户心跳, 如果心跳超时, 就预约删除该用户
     */
    void CheckHeartBeat();
private:
    void On_CS_LOGIN(const chess::SPacket &xPacket);
    void On_CS_REGISTER(const chess::SPacket &xPacket);
    void On_CS_SEND_MSG(const chess::SPacket &xPacket);
    void On_CS_MOVE_PIECE(const chess::SPacket &xPacket);
    void On_CS_TAKESITE();
    void On_CS_REGAME();

    void On_SC_LOGIN_ACK(void *pParam);
    void On_SC_REGISTER_ACK(const void* pParam);
    void On_SC_PUSH_MSG(SEventBuffer *pEventBuffer);
    void On_SC_TAKESITE_ACK(SEventBuffer* pEventBuffer);
    void On_SC_MOVE_PIECE_ACK(SEventBuffer* pEventBuffer);
    void On_SC_PUSH_CHESS_PANEL(SEventBuffer* pEventBuffer);
    void On_SC_PUSH_GAME_OVER(SEventBuffer* pEventBuffer);
    void On_SC_PUSH_TURN_CHANGE(SEventBuffer* pEventBuffer);
    void On_SC_PUSH_USER_LEAVE(SEventBuffer* pEventBuffer);
    void On_SC_PUSH_JIANGJU(SEventBuffer* pEventBuffer);


    void On_CS_HEART_BEAT();

    template <typename SDStruct>
    void PutEventToDBMgr(SDStruct const& xStruct, UINT uEventType)
    {
        SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

        pEventBuffer->Write (m_uConnIndex, uEventType, &xStruct, sizeof(xStruct) );

        g_pDBThreadMgr->QueEvent ( pEventBuffer );
    }
private:
    void OnLoginOK();
private:
    bool  m_bIsValid;                 // 用户是否有效
    UINT m_uConnIndex;         // 用户在用户管理器 && 网络库 的链接索引
    bool m_bIsLogined;              // 是否已经登陆
    std::string m_strUserName; // 用户名
    time_t     m_xLastHeartTime; // 最后一次心跳的时间(s)
};

}

#endif // SRVUSER_H
