#include "SrvUser.h"
#include "SrvUserMgr.h"
#include "RoLog/RoLog.h"
#include "EventBuffer/EventBuffer.h"
#include "Protol/CSProtol.h"
#include "Protol/CSProtol.Parser.hpp"
#include "Protol/CSProtol.Serialize.hpp"
#include <string.h>
#include "DBThread/DBThreadMgr.h"
#include "ChessGame/ChessGame.h"

namespace chess
{
CSrvUser::CSrvUser()
{
    Reset ();
}

void CSrvUser::SetConnIndex(UINT uConnIndex)
{
    m_uConnIndex = uConnIndex;
}

UINT CSrvUser::GetConnIndex() const
{
    return m_uConnIndex;
}

bool CSrvUser::IsValid() const
{
    return m_bIsValid;
}

void CSrvUser::Reset()
{
    m_bIsValid = false;
    m_bIsLogined = false;
}

void CSrvUser::SetValid()
{
    m_bIsValid = true;
}

void CSrvUser::OnRecvData(const char *pBuffer, UINT uBufferBytes)
{
    SPacket xPaket;

    if ( !ParsePacket (xPaket, pBuffer, uBufferBytes) )
    {
        START_LOG(RO_COUT).OUT_LOG("Unkown paket !").END_LOG();
        return ;
    }

    switch (xPaket.m_uEventType)
    {
    case CS_REGISTER:
        On_CS_REGISTER(xPaket);
        break;
    case CS_LOGIN:
        On_CS_LOGIN(xPaket);
        break;
    case CS_SEND_MSG:
        On_CS_SEND_MSG(xPaket);
        break;
    case CS_HEART_BEAT:
        On_CS_HEART_BEAT();
        break;
    case CS_MOVE_PIECE:
        On_CS_MOVE_PIECE(xPaket);
        break;
    case CS_TAKESITE:
        On_CS_TAKESITE();
        break;
    case CS_REGAME:
        On_CS_REGAME();
    default:
        START_LOG(RO_CWARM).OUT_LOG("Unkown EventType ")
                .BUILD_LOG(xPaket.m_uEventType).END_LOG();
        break;
    }
}

void CSrvUser::OnAcceptOK()
{
    m_xLastHeartTime = time(NULL);
}

void CSrvUser::ReverseDelete()
{
    if ( m_bIsValid )
    {
        g_pSrvUserMgr->ReverseDeleteUser ( m_uConnIndex );

        if ( m_bIsLogined )
        {
            g_pChessGame->DoLeave (m_uConnIndex);
        }

        m_bIsValid = false;
        m_bIsLogined = false;

        START_LOG(RO_COUT).OUT_LOG("SrvUser disconnected ... ").BUILD_LOG( m_uConnIndex ).END_LOG();
    }
}

}
