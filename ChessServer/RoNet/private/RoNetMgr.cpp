#include "RoNetMgr.h"
#include "RoLog/RoLog.h"
#include "RoSpace/RoTools.h"

#include <event2/bufferevent.h>
#include <event.h>
#include <event2/listener.h>

namespace RoNetPrivate
{

CRoNetMgr::CRoNetMgr()
{
    m_bIsInited = false;
    m_xEventBase = NULL;
    m_xEventListener = NULL;
    m_xEventTimer = NULL;
    m_xRecvBuffer = NULL;
}

CRoNetMgr::~CRoNetMgr ()
{
    if ( m_bIsInited )
    {
        Free ();
    }
}

bool CRoNetMgr::Init(const RoNet::SRoNetInitData &xData)
{
    if ( m_bIsInited )
    {
        START_LOG(RO_CERR).OUT_LOG("Have inited before !").END_LOG();
        return false;
    }

    if ( xData.m_uRecvBufferByte < 4 )
    {
        START_LOG(RO_CERR).BUILD_LOG(xData.m_uRecvBufferByte).OUT_LOG(" is too small !").END_LOG();
        goto error_proc;
    }
    if ( xData.m_uMaxConnCount == 0 )
    {
        START_LOG(RO_CERR).BUILD_LOG(xData.m_uMaxConnCount).OUT_LOG(" is too small !").END_LOG();
        goto error_proc;
    }

    m_xRecvBuffer = new char[ xData.m_uRecvBufferByte ];

    DoInitSaveData (xData);

    if ( !DoInitNewEventBase () )
    {
        goto error_proc;
    }

    if ( !DoInitNewEventListener (xData.m_uListenPort) )
    {
        goto error_proc;
    }

    DoInitNetUsers (xData.m_uMaxConnCount) ;

    if ( !DoInitNewTimer () )
    {
        goto error_proc;
    }

    START_LOG(RO_COUT).END_LOG();

    m_bIsInited = true;

    START_LOG(RO_COUT).OUT_LOG("Init network ok !").END_LOG();
    return true;

error_proc:
    START_LOG(RO_CERR).OUT_LOG("Init network error !").END_LOG();
    m_bIsInited = true;
    Free();
    return false;
}

void CRoNetMgr::Free()
{
    if ( !m_bIsInited )
    {
        START_LOG(RO_CWARM).BUILD_LOG(m_bIsInited).END_LOG();
        return ;
    }
    m_vecRoNetUsers.clear ();
    m_deqInvalidUsers.clear ();

    if ( m_xEventListener )
    {
        evconnlistener_free ( m_xEventListener );
        m_xEventListener = nullptr;
    }
    if ( m_xEventTimer)
    {
        event_free (m_xEventTimer);
        m_xEventTimer = nullptr;
    }
    if ( m_xEventBase)
    {
        event_base_free ( m_xEventBase );
        m_xEventBase = nullptr;
    }
    SAFE_DELETE_ARRAY( m_xRecvBuffer );

    m_bIsInited = false;

    START_LOG(RO_COUT).OUT_LOG("OK !").END_LOG();
}

bool CRoNetMgr::LogicTick()
{
    m_bHasPacket = false;

    event_base_dispatch ( m_xEventBase );

    return m_bHasPacket;
}

void CRoNetMgr::Disconnect (UINT uConnIndex)
{
    auto pUser = GetConnectedUser ( uConnIndex );

    if ( !pUser )
    {
        return ;
    }
    pUser->Disconnect ();
    m_deqInvalidUsers.push_back ( uConnIndex );

    START_LOG(RO_COUT).OUT_LOG("Disconnect ok !").BUILD_LOG(uConnIndex).END_LOG();
}

bool CRoNetMgr::SendData(UINT uConnIndex, const char *pBuffer, UINT uBufferBytes)
{
    auto pUser = GetConnectedUser ( uConnIndex );
    if ( !pUser )
    {
        START_LOG(RO_CERR).BUILD_LOG(pUser).END_LOG();
        return false;
    }
    return 0 == bufferevent_write (pUser->GetBufferEvent (), pBuffer, uBufferBytes);
}

_SRoNetUser* CRoNetMgr::GetConnectedUser (UINT uConnIndex)
{
    if ( !IsValidConnIndex (uConnIndex) )
    {
        START_LOG(RO_CERR).OUT_LOG("ConnIndex is error !").BUILD_LOG(uConnIndex).END_LOG();
        return NULL;
    }

    _SRoNetUser& user = m_vecRoNetUsers[ uConnIndex ];

    if ( !user.IsConnected () )
    {
        START_LOG(RO_CERR).OUT_LOG("User is not valid !").BUILD_LOG(uConnIndex).END_LOG();
        return NULL;
    }

    return &user;
}

void CRoNetMgr::_OnAcceptOK (evconnlistener *, int fd, sockaddr *, int, void *)
{
    g_pRoNetMgr->OnAccepkOK (fd);
}

void CRoNetMgr::OnAccepkOK (int fd)
{
    UINT uConnIndex = 0;

    if ( m_deqInvalidUsers.empty () )
    {
        START_LOG(RO_CWARM).OUT_LOG("Connection is full !").END_LOG();
        evutil_closesocket ( fd );
        return ;
    }

    uConnIndex = m_deqInvalidUsers.front ();
    m_deqInvalidUsers.pop_front ();

    _SRoNetUser& user = m_vecRoNetUsers[uConnIndex];

    user.SetBufferEvent ( DoSetupNewBufferevent(fd, user.GetConnIndexPtr ())  );

    if ( !m_fpOnAcceptOK )
    {
        START_LOG(RO_CWARM).BUILD_LOG( m_fpOnAcceptOK ).END_LOG();
    }
    else
    {
        m_fpOnAcceptOK( uConnIndex );
    }
    START_LOG(RO_COUT).BUILD_LOG(uConnIndex).END_LOG();
    return ;
}

void CRoNetMgr::_OnBevRead (bufferevent *, void *arg)
{
    g_pRoNetMgr->OnRead( *(UINT*)arg );
}

void CRoNetMgr::_OnBevError (bufferevent *, short , void *arg)
{
    g_pRoNetMgr->OnError ( *(UINT*)arg );
}

void CRoNetMgr::_OnTimer(int, short, void *)
{
    g_pRoNetMgr->OnTimer();
}

void CRoNetMgr::OnRead (UINT uConnIndex)
{
    _SRoNetUser& user = m_vecRoNetUsers[ uConnIndex ];
    auto evb = user.GetBufferEvent ()->input;
    size_t uBytesAvaliable = evbuffer_get_length ( evb );

    // 应该接收数据头部信息
    constexpr size_t uPacketHeaderBytes = 4;

    if ( uBytesAvaliable < uPacketHeaderBytes )
    {
        return ;
    }
    UINT uPacketBytes =0;
    {
        UINT uPacketBytesNetOrder;

        evbuffer_copyout (evb,  &uPacketBytesNetOrder, uPacketHeaderBytes);

        uPacketBytes = ntohl ( uPacketBytesNetOrder );
    }

    if ( uPacketBytes > m_uRecvBufferByte || // 包字节数超过了接收缓冲字节数, 包异常
         uPacketBytes < uPacketHeaderBytes ) // 包整体大小比包头部占用都小, 当然包有异常
    {
        START_LOG(RO_CERR).OUT_LOG("Packet exception !").BUILD_LOG( m_uRecvBufferByte )
                .BUILD_LOG( uPacketBytes ).END_LOG();
        OnError ( uConnIndex );
        return ;
    }

    if ( uBytesAvaliable < uPacketBytes )
    {
        return ;
    }

    // 读取一个数据包
    bufferevent_read(user.GetBufferEvent (), m_xRecvBuffer, uPacketBytes );

    m_bHasPacket = true;

    if ( !m_fpOnRecvData )
    {
        START_LOG(RO_CWARM).BUILD_LOG(m_fpOnRecvData).END_LOG();
        return ;
    }

    m_fpOnRecvData( uConnIndex, m_xRecvBuffer, uPacketBytes);
}

void CRoNetMgr::OnError (UINT uConnIndex)
{
    auto bev = GetConnectedUser ( uConnIndex )->GetBufferEvent ();

    bufferevent_disable (bev, EV_READ);

    if ( !m_fpOnDisconnected )
    {
        START_LOG(RO_CWARM ).BUILD_LOG(m_fpOnDisconnected).END_LOG();
        return ;
    }
    m_fpOnDisconnected( uConnIndex );
}

void CRoNetMgr::OnTimer ()
{
    if ( !m_fpAppLogic )
    {
        START_LOG(RO_CWARM).BUILD_LOG(m_fpAppLogic).END_LOG();
        event_base_loopbreak (m_xEventBase);
        return ;
    }
    event_base_loopbreak (m_xEventBase);

    m_fpAppLogic();
}

void CRoNetMgr::DoInitSaveData(const SRoNetInitData &xData)
{
    m_fpAppLogic = xData.m_fpAppLogic;
    m_fpOnAcceptOK = xData.m_fpOnAcceptOK;
    m_fpOnDisconnected = xData.m_fpOnDisconnected;
    m_fpOnRecvData = xData.m_fpOnRecvData;
    m_uRecvBufferByte = xData.m_uRecvBufferByte;
}

bool CRoNetMgr::DoInitNewEventBase()
{
    m_xEventBase = event_base_new ();

    if ( !m_xEventBase )
    {
        START_LOG( RO_CERR ).BUILD_LOG( m_xEventBase ).END_LOG();

        return false;
    }
    return true;
}

bool CRoNetMgr::DoInitNewEventListener(UINT uListenPort)
{
    struct sockaddr_in sockaddrListen;
    char xBuffer[ BUFSIZ ];

    snprintf (xBuffer, BUFSIZ, "0.0.0.0:%u", uListenPort);
    int iListenAddrLen = sizeof( sockaddrListen );

    if ( 0 != evutil_parse_sockaddr_port (xBuffer,(sockaddr*) &sockaddrListen, &iListenAddrLen) )
    {
        START_LOG(RO_CERR).BUILD_LOG( xBuffer ).END_LOG ();
        return false;
    }
    m_xEventListener = evconnlistener_new_bind (m_xEventBase,
                                                _OnAcceptOK,
                                                NULL,
                                                LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
                                                5,
                                                (sockaddr*)&sockaddrListen,
                                                iListenAddrLen);
    if ( !m_xEventListener )
    {
        START_LOG(RO_CERR).BUILD_LOG( m_xEventListener ).BUILD_LOG( uListenPort ).END_LOG();

        return false;
    }

    START_LOG(RO_COUT).OUT_LOG("Init listener ok : ").BUILD_LOG(uListenPort).END_LOG();
    return true;
}

void CRoNetMgr::DoInitNetUsers(UINT uMaxConnCount)
{
    {
        _SRoNetUser user;
        m_vecRoNetUsers.resize (uMaxConnCount + 1, user);
    }

    for(UINT u=1; u<m_vecRoNetUsers.size (); ++u)
    {
        _SRoNetUser& user = m_vecRoNetUsers[u];
        user.SetConnIndex ( u );
        m_deqInvalidUsers.push_back (u);
    }
}

bool CRoNetMgr::DoInitNewTimer()
{
    m_xEventTimer = event_new (m_xEventBase, -1, EV_PERSIST, _OnTimer, NULL);

    if ( !m_xEventTimer )
    {
        START_LOG(RO_CERR).BUILD_LOG(m_xEventTimer).END_LOG();
        return false;
    }

    /**
      * 每10毫秒调用一次AppLogic进行逻辑处理
     */
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec= 10000;

    evtimer_add(m_xEventTimer, &tv);

    return true;
}

bufferevent *CRoNetMgr::DoSetupNewBufferevent(int fd, void *arg)
{
    auto bev = bufferevent_socket_new (m_xEventBase, fd, BEV_OPT_CLOSE_ON_FREE);

    if ( !bev )
    {
        START_LOG(RO_CERR).END_LOG();
        return NULL;
    }

    bufferevent_settimeout (bev, 0, 3);

    bufferevent_setwatermark (bev, EV_READ, 0, m_uRecvBufferByte);
    bufferevent_setcb (bev, _OnBevRead, NULL, _OnBevError, arg) ;
    bufferevent_enable (bev, EV_READ);

    return bev;
}

}
