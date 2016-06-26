#ifndef SRVUSERMGR_H
#define SRVUSERMGR_H

#include "RoSpace/RoSingleton.h"
#include "RoSpace/RoTypes.h"
#include <vector>
#include <queue>
#include <mutex>
#include <unordered_set>

namespace chess
{
class SEventBuffer;

using namespace RoSpace;
class CSrvUser;

class CSrvUserMgr
        : public RoSpace::CSingleton<CSrvUserMgr>
{
public:
    CSrvUserMgr();
    ~CSrvUserMgr();

    /**
     * @brief Init 初始化用户管理器
     * @param uMaxUserCount 最到用户数量
     * @return true -> 初始化成功, false -> 初始化失败
     */
    bool Init(UINT uMaxUserCount);

    /**
     * @brief GetUserPtr 从用户池中获取一个用户指针
     * @param uConnIndex 用户索引
     * @return 如果索引非法则返回 NULL, 如果索引合法则返回对应的用户指针
     */
    CSrvUser* GetUserPtr(UINT uConnIndex);
    /**
     * @brief GetValidUserPtr 获取一个有效的用户指针 [有效用户就是已经链接到服务器的用户]
     * @param uConnIndex 用户索引
     * @return 如果索引非法或者用户未链接返回NULL, 否则返回对应用户指针
     */
    CSrvUser* GetValidUserPtr(UINT uConnIndex);

    /**
     * @brief GetLoginedUserNames 获取已经登陆的用户列表
     * @return 登陆的用户列表
     */
    std::unordered_set<std::string> const& GetLoginedUserNames() const;

    /**
     * @brief LogicTick 用户管理器的逻辑处理,
     *  1. 检查心跳
     *  2. 删除预约断开的所有用户
     *  3. 处理需要写到用户的事件
     */
    void LogicTick();

    /**
     * @brief OnAcceptOK 网络库接收到一个新链接的响应函数
     * @param uConnIndex
     */
    void OnAcceptOK(UINT uConnIndex);

    /**
     * @brief ReverseDeleteUser 预约删除用户
     * @param uConnIndex
     */
    void ReverseDeleteUser(UINT uConnIndex)
    {
        m_vecReverseDeleteUsers.push_back ( uConnIndex );
    }

    /**
     * @brief QueueEvent 数据库线程投递一个事件到用户管理器
     * @param pEventBuffer 需要投递的事件
     */
    void QueueEvent(SEventBuffer* pEventBuffer);

    /**
     * @brief AddLoginUser 用户对象将自己设置为已经登陆的用户, 还需要到用户管理器设置
     * @param pUser
     */
    void AddLoginUser(CSrvUser* pUser);

    /**
     * @brief AddValidUser 添加一个有效用户
     * @param pUser
     */
    void AddValidUser(CSrvUser* pUser);
    /**
     * @brief IsLoginedUser 用户对象判断一个用户是否已经登陆
     * @param strUserName 用户名
     * @return true -> 已登陆, false -> 未登陆
     */
    bool IsLoginedUser(std::string const& strUserName) const;

    /**
     * @brief LoginedUsersProcessEvent 已登陆的用户处理事件 [用于发送广播]
     * @param pEventBuffer 事件缓冲区
     */
    void LoginedUsersProcessEvent(SEventBuffer *pEventBuffer);

private:
    /**
     * @brief IsValidIndex 判断一个索引是不是有效索引 (0, m_uUserPoolSize) 为有效索引
     * @param uConnIndex 待判断的索引
     * @return true-> 是有效索引, false -> 不是有效索引
     */
    bool IsValidIndex(UINT uConnIndex)
    {
        return uConnIndex && uConnIndex < m_uUserPoolSize;
    }

    /**
     * @brief RemoveValidUser 移除一个有效用户
     * @param pUser 有效用户
     */
    void RemoveValidUser(CSrvUser* pUser);

    /**
     * @brief RemoveLoginedUser 移除已经登陆的用户记录
     * @param pUser 用户
     */
    void RemoveLoginedUser(CSrvUser* pUser);

    /**
     * @brief CheckUserHeartBeat 检查用户心跳
     */
    void CheckUserHeartBeat();
    /**
     * @brief CheckDeleteUsers 统一通知网络库断开所有预约删除的用户
     */
    void CheckDeleteUsers();

    /**
     * @brief ProcessAllEvent 处理所有用户事件[注册回应, 登陆回应]
     */
    void ProcessAllEvent();

    /**
     * @brief PushUserStatusChanges 推送用户状态到每个已经登陆的用户
     */
    void PushUserStatusChanges();

    /**
     * @brief GetEventBuffer 从事件队列中获取一个事件
     * @return 事件, 事件队列为空时, 返回NULL
     */
    SEventBuffer* GetEventBuffer();

private:
    CSrvUser* m_xUserPool; // 用户池
    UINT          m_uUserPoolSize; // 用户池容量
    std::unordered_set<CSrvUser*> m_usetValidUser; // 有效用户, 用于快速遍历
    std::unordered_set<CSrvUser*> m_usetLoginedUser; // 登陆了的用户, 用于快速遍历
    std::vector<UINT>         m_vecReverseDeleteUsers; // 预约删除的用户
    std::unordered_set<std::string> m_usetLoginedUserName; // 登陆用户

    std::unordered_set<std::string> m_usetLoginedThisTime; // 本次登陆的用户
    std::unordered_set<std::string> m_usetDisconnectedThisTime; // 本次断开的用户

    std::queue<SEventBuffer*> m_queUserEvents; // 需要用户处理的事件
    std::mutex                               m_mtxSyncUserEvent; // 事件队列互斥量
};

#define g_pSrvUserMgr chess::CSrvUserMgr::Instance ()

}
#endif // SRVUSERMGR_H
