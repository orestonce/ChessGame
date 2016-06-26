#include "RoNetUser.h"
#include "RoLog/RoLog.h"
#include <event.h>
#include <event2/bufferevent.h>

namespace RoNetPrivate
{
_SRoNetUser::~_SRoNetUser ()
{
    if ( IsConnected () )
    {
        bufferevent_free ( m_xBufferEvent );
    }
}

bufferevent *_SRoNetUser::GetBufferEvent() const
{
    return m_xBufferEvent;
}

void _SRoNetUser::SetBufferEvent(bufferevent *bev)
{
    m_xBufferEvent = bev;
}

void _SRoNetUser::SetConnIndex(UINT uConnIndex)
{
    m_uConnIndex = uConnIndex;
}

UINT *_SRoNetUser::GetConnIndexPtr()
{
    return &m_uConnIndex;
}

void _SRoNetUser::Disconnect()
{
    if ( !IsConnected () )
    {
        START_LOG(RO_CERR).OUT_LOG( " net user is not connected !" ).END_LOG();
        return ;
    }
    bufferevent_free ( m_xBufferEvent );
    m_xBufferEvent = nullptr;
}


_SRoNetUser::_SRoNetUser()
{
    m_xBufferEvent = NULL;
}


bool _SRoNetUser::IsConnected() const
{
    return m_xBufferEvent != NULL;
}

}
