#ifndef RONETMGR_H
#define RONETMGR_H
#include "RoNet/RoNet.h"
#include "RoSpace/RoSingleton.h"
#include "RoNetUser.h"
#include <vector>
#include <deque>

struct bufferevent;
struct evconnlistener;
struct event_base;
struct sockaddr;
struct event;

namespace RoNetPrivate
{
using namespace RoNet;

class CRoNetMgr
        : public RoSpace::CSingleton<CRoNetMgr>
{
public:

    CRoNetMgr();
    ~CRoNetMgr();
    /**
     * @brief Init 初始化网络库
     * @param xData 初始化数据
     * @return true -> 初始化成功, false -> 初始化失败
     */
    bool Init(const SRoNetInitData& xData);
    /**
     * @brief Free 释放网络库
     */
    void Free();
    /**
     * @brief LogicTick 一次网络逻辑处理, 调用一次AppLogic
     * @return true -> 收到过新的数据包, flase -> 没有收到新的数据包
     */
    bool LogicTick();

    /**
     * @brief Disconnect
     * @param uConnIndex
     */
    void Disconnect(UINT uConnIndex);

    /**
     * @brief SendData          发送数据
     * @param uConnIndex  链接索引
     * @param pBuffer          数据缓冲
     * @param uBufferBytes 数据大小
     * @return true -> 写到了网络缓冲, false -> 网络缓冲不足
     */
    bool SendData(UINT uConnIndex, const char *pBuffer, UINT uBufferBytes);

private:
    /**
     * @brief GetConnectedUser 获取一个已经建立链接的用户
     * @param uConnIndex 尝试获得的用户索引
     * @return 成功返回已经建立链接的用户指针, 失败返回NULL
     */
    _SRoNetUser* GetConnectedUser(UINT uConnIndex);

    /**
     * @brief IsValidConnIndex 判断一个链接索引是否有效
     * @param uConnIndex 待判断的链接索引
     * @return true -> 有效, false -> 无效
     */
    bool IsValidConnIndex(UINT uConnIndex) const
    {
        return uConnIndex && uConnIndex <= m_vecRoNetUsers.size ();
    }
private:
    /**
     * @brief _OnAcceptOK 接收到新链接的回调函数
     * @param pListener 监听器
     * @param fd 新到来的文件描述符
     * @param pNewSocketAddr 新到来的地址
     * @param socklen 地址长度
     * @param arg 用户自定义参数
     */
    static void _OnAcceptOK(evconnlistener *, int fd, sockaddr *, int , void *);

    /**
     * @brief _OnBevRead 某个BufferEvent可读的回调函数
     * @param arg 用户自定义参数, 在这里为UINT*， 解引用结果为用户的链接索引
     */
    static void _OnBevRead(bufferevent*, void *arg);
    /**
     * @brief _OnBevError 某个BufferEvent错误的回调函数
     * @param arg 用户自定义参数, 在这里为UINT*， 解引用结果为用户的链接索引
     */
    static void _OnBevError(bufferevent*, short, void *arg);
    /**
     * @brief _OnTimer 计时器事件回调函数, 这里指的是应该调用AppLogic了
     */
    static void _OnTimer(int , short, void* );
    /**
     * @brief OnAccepkOK 接收链接成功的处理函数
     * @param fd 新来的链接的文件描述符
     */
    void OnAccepkOK(int fd);

    /**
     * @brief OnRead 可读事件发生的处理函数
     * @param uConnIndex 发生事件的用户的索引
     */
    void OnRead(UINT uConnIndex);
    /**
     * @brief OnError 错误事件发生的处理函数
     * @param uConnIndex 发生错误事件的用户索引
     */
    void OnError(UINT uConnIndex);
    /**
     * @brief OnTimer 计事器事件的处理函数
     */
    void OnTimer();

private:

    // 一些初始化工具
    void DoInitSaveData(const SRoNetInitData& xData );
    bool DoInitNewEventBase();
    bool DoInitNewEventListener(UINT uListenPort);
    void DoInitNetUsers(UINT uMaxConnCount);
    bool DoInitNewTimer();
    bufferevent* DoSetupNewBufferevent(int fd, void *arg);

private:
    bool                                        m_bIsInited;               // Init && 没有被 Free
    FP_AppLogic                         m_fpAppLogic;           // 应用逻辑
    FP_OnAcceptOK                   m_fpOnAcceptOK;      // 接收链接成功
    FP_OnDisconnected             m_fpOnDisconnected; // 对方断开了链接
    FP_OnRecvData                    m_fpOnRecvData;       // 接收到一个数据包
    UINT                                      m_uRecvBufferByte;   // 接收缓冲大小
    char                                      *m_xRecvBuffer;           // 接收缓冲区

    event_base                           *m_xEventBase;            // 事件源
    evconnlistener                     *m_xEventListener;       // 监听事件
    event                                    *m_xEventTimer;           // 定时器事件
    std::vector<_SRoNetUser> m_vecRoNetUsers;        // 所有的网络用户
    std::deque<UINT>              m_deqInvalidUsers;      // 无效的用户
    bool                                        m_bHasPacket;             // 本次LogicTick是否接收到了封包
};

#define g_pRoNetMgr RoNetPrivate::CRoNetMgr::Instance ()

}
#endif // RONETMGR_H
