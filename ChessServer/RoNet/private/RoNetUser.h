#ifndef RONETUSER_H
#define RONETUSER_H

#include "RoSpace/RoTypes.h"

struct bufferevent;

namespace RoNetPrivate
{

using namespace RoSpace;

/**
 * @brief The _SRoNetUser struct 一个网络用户
 * 该类使用要极其小心 !
 *  1. 不要复制 !! 除非 m_xBufferEvent 为NULL 否则会出现多次使用bufferevent的情况
 *  2. 在SetBufferEvent使用时必须确保m_xBufferEvent 为NULL或者已经被释放
 *  3. 不建议使用 GetConnIndexPtr()的返回值去修改m_uConnIndex
 */
struct _SRoNetUser
{
public :
    /**
     * @brief _SRoNetUser 构造一个空的_SRoNetUser
     */
    _SRoNetUser();
    /**
      * 如果有BufferEvent，则free
      */
    ~_SRoNetUser();

    /**
     * @brief GetBufferEvent 获取对应的BufferEvent
     * @return 该用户的BufferEvent
     */
    bufferevent* GetBufferEvent() const;
    /**
     * @brief SetBufferEvent 设置该用户的BufferEvent
     * @param bev 新的BufferEvent
     */
    void SetBufferEvent(bufferevent* bev);
    /**
     * @brief SetConnIndex 设置链接索引
     * @param uConnIndex 对应的链接索引
     */
    void SetConnIndex(UINT uConnIndex);
    /**
     * @brief GetConnIndexPtr 获取对应的链接索引指针
     * @return 链接索引的指针
     */
    UINT *GetConnIndexPtr();
    /**
     * @brief IsConnected 判断该用户是否与远程建立链接
     * @return true -> 有链接, false -> 无链接
     */
    bool IsConnected() const;

    /**
     * @brief Disconnect 断开链接, 但不释放buffer_event[ 以便以后重用BufferEvent ]
     */
    void Disconnect ();
private:
    UINT             m_uConnIndex; // 链接索引[便于g_pRoNetMgr取得该用户]
    bufferevent *m_xBufferEvent; // 缓冲事件
};

}

#endif // RONETUSER_H
