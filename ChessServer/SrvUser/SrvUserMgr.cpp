#include "SrvUserMgr.h"
#include "RoSpace/RoTools.h"
#include "SrvUser.h"
#include "Protol/CSProtol.h"
#include "RoNet/RoNet.h"
#include "RoLog/RoLog.h"
#include "DBThread/DBThreadMgr.h"
#include "EventBuffer/EventBufferRecycle.h"
#include "EventBuffer/EventBuffer.h"
#include "Protol/CSProtol.Serialize.hpp"
#include "Protol/CSProtol.SendData.hpp"
#include <algorithm>

namespace chess
{

CSrvUserMgr::CSrvUserMgr()
{
    m_xUserPool = nullptr;
}

CSrvUserMgr::~CSrvUserMgr ()
{
    SAFE_DELETE_ARRAY( m_xUserPool );
}

bool CSrvUserMgr::Init(UINT uMaxUserCount)
{
    if ( m_xUserPool )
    {
        START_LOG(RO_CERR).OUT_LOG("You have inited this CSrvUserMgr !").END_LOG();
        return false;
    }
    // 多分配一个用户, 以便和网络库配合
    m_uUserPoolSize = uMaxUserCount + 1;
    m_xUserPool = new CSrvUser[ m_uUserPoolSize ];

    for(UINT uConnIndex=1; uConnIndex<m_uUserPoolSize; ++uConnIndex)
    {
        m_xUserPool[ uConnIndex ].SetConnIndex ( uConnIndex );
    }

    return true;
}

CSrvUser *CSrvUserMgr::GetUserPtr(UINT uConnIndex)
{
    if ( IsValidIndex (uConnIndex) )
    {
        return m_xUserPool + uConnIndex;
    }
    return NULL;
}

CSrvUser *CSrvUserMgr::GetValidUserPtr(UINT uConnIndex)
{
    CSrvUser* pUser = GetUserPtr ( uConnIndex );

    if ( pUser && pUser->IsValid() )
    {
        return pUser;
    }

    return NULL;
}

const std::unordered_set<std::string> &CSrvUserMgr::GetLoginedUserNames() const
{
    return m_usetLoginedUserName;
}

void CSrvUserMgr::LogicTick()
{
    CheckUserHeartBeat();
    CheckDeleteUsers ();
    ProcessAllEvent ();

    PushUserStatusChanges();
}

void CSrvUserMgr::OnAcceptOK(UINT uConnIndex)
{
    CSrvUser* pUser= GetUserPtr (uConnIndex);

    if( pUser->IsValid () )
    {
        START_LOG(RO_CERR).OUT_LOG("User is valid ! ").BUILD_LOG( uConnIndex ).END_LOG();
        RoNet::Disconnect ( uConnIndex );
        return ;
    }
    AddValidUser (pUser);
    pUser->Reset ();
    pUser->SetValid ();
    pUser->OnAcceptOK();
}

void CSrvUserMgr::QueueEvent(SEventBuffer *pEventBuffer)
{
    std::lock_guard<std::mutex> lk(m_mtxSyncUserEvent);
    m_queUserEvents.push ( pEventBuffer );
}

void CSrvUserMgr::AddLoginUser(CSrvUser *pUser)
{
    m_usetLoginedUser.insert (pUser);
    m_usetLoginedUserName.insert (pUser->GetUserName ());
    m_usetLoginedThisTime.insert (pUser->GetUserName ());
    m_usetDisconnectedThisTime.erase (pUser->GetUserName ());
}

void CSrvUserMgr::AddValidUser(CSrvUser *pUser)
{
    m_usetValidUser.insert (pUser);
}

bool CSrvUserMgr::IsLoginedUser(const std::string &strUserName) const
{
    return m_usetLoginedUserName.find (strUserName) != m_usetLoginedUserName.end ();
}

void CSrvUserMgr::RemoveValidUser(CSrvUser *pUser)
{
    m_usetValidUser.erase (pUser);
}

void CSrvUserMgr::RemoveLoginedUser(CSrvUser *pUser)
{
    m_usetLoginedUserName.erase (pUser->GetUserName ());
    if ( m_usetLoginedUser.erase (pUser) )
    {
        m_usetDisconnectedThisTime.insert ( pUser->GetUserName () );
        m_usetLoginedThisTime.erase (pUser->GetUserName ());
    }
}

void CSrvUserMgr::CheckUserHeartBeat()
{
    for(CSrvUser* pUser : m_usetValidUser)
    {
        pUser->CheckHeartBeat();
    }
}

void CSrvUserMgr::CheckDeleteUsers()
{
    if ( m_vecReverseDeleteUsers.empty () )
    {
        return ;
    }

    for(UINT uConnIndex : m_vecReverseDeleteUsers )
    {
        CSrvUser* pUser = GetUserPtr (uConnIndex);

        if ( pUser )
        {
            std::cout << "Disconnect : " << uConnIndex << std::endl;

            RemoveValidUser (pUser);
            RemoveLoginedUser(pUser);

            RoNet::Disconnect ( uConnIndex );
        }
    }
    m_vecReverseDeleteUsers.clear ();
}

void CSrvUserMgr::ProcessAllEvent()
{
    SEventBuffer* pEventBuffer;

    while ( (pEventBuffer = GetEventBuffer()) != NULL )
    {
        if ( pEventBuffer->m_uTraceID == 0 ) // send to all
        {
            LoginedUsersProcessEvent (pEventBuffer);
        }
        else
        {
            CSrvUser* pUser = GetValidUserPtr ( pEventBuffer->m_uTraceID );
            if ( pUser )
            {
                pUser->ProcessEvent( pEventBuffer );
            }
        }
        g_pEventBufferRecycle->RecycleEventBuffer (pEventBuffer);
    }
}

void CSrvUserMgr::LoginedUsersProcessEvent(SEventBuffer *pEventBuffer)
{
    for(CSrvUser* pUser : m_usetLoginedUser)
    {
        pUser->ProcessEvent ( pEventBuffer );
    }
}

void CSrvUserMgr::PushUserStatusChanges()
{
    if ( !m_usetLoginedThisTime.empty () )
    {
        SSC_USER_LOGINED xLoginedUsers;

        xLoginedUsers.m_vecLoginUsers.assign ( m_usetLoginedThisTime.begin (),
                                               m_usetLoginedThisTime.end ());

        for(CSrvUser* pUser : m_usetLoginedUser)
        {
            SendData(pUser->GetConnIndex (), xLoginedUsers, SC_USER_LOGINED);
        }
        m_usetLoginedThisTime.clear ();
    }

    if ( !m_usetDisconnectedThisTime.empty () )
    {
        SSC_USER_DISCONNECTED xDisconnUsers;

        xDisconnUsers.m_vecDisconnectedUsers.assign (m_usetDisconnectedThisTime.begin (),
                                                     m_usetDisconnectedThisTime.end ());

        for(CSrvUser* pUser : m_usetLoginedUser)
        {
            SendData(pUser->GetConnIndex (), xDisconnUsers, SC_USER_DISCONNECTED);
        }
        m_usetDisconnectedThisTime.clear ();
    }
}

SEventBuffer *CSrvUserMgr::GetEventBuffer()
{
    std::lock_guard<std::mutex> lk(m_mtxSyncUserEvent);

    if ( m_queUserEvents.empty () )
    {
        return NULL;
    }
    SEventBuffer* pEventBuffer = m_queUserEvents.front ();
    m_queUserEvents.pop ();
    return pEventBuffer;
}

}
