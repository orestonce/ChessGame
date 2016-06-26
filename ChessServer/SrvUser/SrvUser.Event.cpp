#include "SrvUser.h"
#include "SrvUser/SrvUserMgr.h"
#include "Protol/Common.h"
#include "Protol/CSProtol.h"
#include "Protol/CSProtol.Serialize.hpp"
#include "Protol/CSProtol.Parser.hpp"
#include "Protol/CSProtol.SendData.hpp"
#include "EventBuffer/EventBuffer.h"
#include "DBThread/DBThreadMgr.h"
#include "EventBuffer/EventBuffer.h"
#include "EventBuffer/EventBufferRecycle.h"
#include "Protol/ServerInternal.h"
#include "SrvConfig/CSrvConfig.h"
#include <time.h>
#include "ChessGame/ChessGame.h"

namespace chess
{

void CSrvUser::ProcessEvent(SEventBuffer *pEventBuffer)
{
    switch ( pEventBuffer->m_uEventType )
    {
    case SC_LOGIN_ACK:
        On_SC_LOGIN_ACK(pEventBuffer->GetEventParamPtr ());
        break;
    case SC_REGISTER_ACK:
        On_SC_REGISTER_ACK(pEventBuffer->GetEventParamPtr ());
        break;
    case SC_PUSH_MSG:
        On_SC_PUSH_MSG(pEventBuffer);
        break;
    case SC_TAKESITE_ACK:
        On_SC_TAKESITE_ACK(pEventBuffer);
        break;
    case SC_MOVE_PIECE_ACK:
        On_SC_MOVE_PIECE_ACK(pEventBuffer);
        break;
    case SC_PUSH_CHESS_PANEL:
        On_SC_PUSH_CHESS_PANEL(pEventBuffer);
        break;
    case SC_PUSH_GAME_OVER:
        On_SC_PUSH_GAME_OVER (pEventBuffer);
        break;
    case SC_PUSH_TURN_CHANGE:
        On_SC_PUSH_TURN_CHANGE (pEventBuffer);
        break;
    case SC_PUSH_USER_LEAVE:
        On_SC_PUSH_USER_LEAVE(pEventBuffer);
        break;
    case SC_PUSH_JIANGJU:
        On_SC_PUSH_JIANGJU(pEventBuffer);
        break;
    default:
        START_LOG(RO_CERR).OUT_LOG("Unkown EventType : ")
                .BUILD_LOG( pEventBuffer->m_uEventType ).END_LOG();
        break;
    }
}

void CSrvUser::CheckHeartBeat()
{
    int iHeartBeatOverTime = g_pSrvConfig->GetHeartBeatOverTime() ;

    if ( time(NULL) - m_xLastHeartTime > iHeartBeatOverTime )
    {
        START_LOG(RO_CERR).BUILD_LOG( time(NULL) - m_xLastHeartTime)
                .OUT_LOG("HeartBeat time over !") .BUILD_LOG(m_strUserName).BUILD_LOG(m_uConnIndex).END_LOG();
        ReverseDelete ();
    }
}

void CSrvUser::On_CS_LOGIN(const SPacket &xPacket)
{
    if ( m_bIsLogined )
    {
        START_LOG(RO_CERR).OUT_LOG("User hase logined !").END_LOG();
        return ;
    }

    SCS_LOGIN xLogin;

    if ( !GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xLogin) )
    {
        START_LOG(RO_CERR).OUT_LOG("Can't parse login data !").END_LOG();
        return ;
    }

    xLogin.m_szUserName[ MAX_USERNAME_LEN ] = '\0';
    xLogin.m_szPassword [ MAX_PASSWORD_LEN ] = '\0';

    if ( g_pSrvUserMgr->IsLoginedUser (xLogin.m_szUserName) )
    {
        SSC_LOGIN_ACK xLoginACK;

        memcpy (xLoginACK.m_szUserName, xLogin.m_szUserName, sizeof(xLoginACK.m_szUserName));
        xLoginACK.m_uLoginResult = SSC_LOGIN_ACK::USER_LOGINED;

        SendData(m_uConnIndex, xLoginACK, SC_LOGIN_ACK);
        return ;
    }

    PutEventToDBMgr(xLogin, CS_LOGIN);
}

void CSrvUser::On_CS_REGISTER(const SPacket &xPacket)
{
    if( m_bIsLogined )
    {
        START_LOG(RO_CERR).OUT_LOG("You have logined !").BUILD_LOG(m_strUserName).END_LOG();
        return ;
    }

    SCS_REGISTER xRegister;

    if ( !GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xRegister) )
    {
        START_LOG(RO_CERR).OUT_LOG("Get register data error !").END_LOG();
        return ;
    }
    xRegister.m_szUserName[ MAX_USERNAME_LEN ] = '\0';
    xRegister.m_szPassword [ MAX_PASSWORD_LEN  ] = '\0';

    PutEventToDBMgr(xRegister, CS_REGISTER);

    START_LOG(RO_COUT).BUILD_LOG(xRegister.m_szUserName)
            .BUILD_LOG(xRegister.m_szPassword).END_LOG();
}

void CSrvUser::On_CS_SEND_MSG(const SPacket &xPacket)
{
    if ( !m_bIsLogined )
    {
        START_LOG(RO_CERR).OUT_LOG("You haven't login !").END_LOG();
        return ;
    }

    SCS_SEND_MSG xSendMsg;

    if ( !GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xSendMsg) )
    {
        START_LOG(RO_CERR).OUT_LOG("Get send-msg param error !").END_LOG();
        return ;
    }
    xSendMsg.m_szMessageBody[ MAX_MSG_LEN ] = '\0';

    // 投递一个推送消息的事件到用户管理器, 以便下次CSrvUserMgr::ProcessAllEvent()
    //  的时候推送给其他用户
    {
        SSC_PUSH_MSG xPushMsg;

        strncpy (xPushMsg.m_szMessageFrom, m_strUserName.c_str (),
                 sizeof(xPushMsg.m_szMessageFrom));
        memcpy (xPushMsg.m_szMessageBody, xSendMsg.m_szMessageBody,
                sizeof(xPushMsg.m_szMessageBody));

        SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

        pEventBuffer->Write (0, SC_PUSH_MSG,
                             &xPushMsg, sizeof(xPushMsg));
        g_pSrvUserMgr->QueueEvent (pEventBuffer);
    }

    // 保存消息到数据库
    {
        SSD_SAVE_MSG xSaveMsg;

        strncpy (xSaveMsg.m_szMessageFrom, m_strUserName.c_str (),
                 sizeof(xSaveMsg.m_szMessageFrom));
        memcpy (xSaveMsg.m_szMessageBody, xSendMsg.m_szMessageBody,
                sizeof(xSaveMsg.m_szMessageBody));

        PutEventToDBMgr(xSaveMsg, SD_SAVE_MSG);
    }
}

void CSrvUser::On_CS_MOVE_PIECE(const SPacket &xPacket)
{
    if ( !m_bIsLogined )
    {
        return ;
    }

    SCS_MOVE_PIECE xMovePiece;

    if ( !GetStruct(xPacket.pParamBegin, xPacket.pParamEnd, xMovePiece) )
    {
        START_LOG(RO_CERR).END_LOG();
        return ;
    }

    CPoint xFrom(xMovePiece.m_xFrom.m_iLine, xMovePiece.m_xFrom.m_iRow);
    CPoint xTo(xMovePiece.m_xTo.m_iLine, xMovePiece.m_xTo.m_iRow);

    START_LOG(RO_COUT)
            .BUILD_LOG(xMovePiece.m_xFrom.m_iLine)
            .BUILD_LOG(xMovePiece.m_xFrom.m_iRow)
            .BUILD_LOG(xMovePiece.m_xTo.m_iLine)
            .BUILD_LOG(xMovePiece.m_xTo.m_iRow)
            .END_LOG();

    g_pChessGame->DoMovePiece (m_uConnIndex, xFrom, xTo);
}

void CSrvUser::On_CS_TAKESITE()
{
    if ( !m_bIsLogined )
    {
        return ;
    }

    g_pChessGame->DoEnterRoom ( m_uConnIndex );
}

void CSrvUser::On_CS_REGAME()
{
    g_pChessGame->DoRegame(m_uConnIndex);
}

void CSrvUser::On_SC_LOGIN_ACK(void *pParam)
{
    SSC_LOGIN_ACK *pLoginAck = static_cast<SSC_LOGIN_ACK*>(pParam);

    if ( pLoginAck->m_uLoginResult == SSC_LOGIN_ACK::LOGIN_OK )
    {
        if ( g_pSrvUserMgr->IsLoginedUser (pLoginAck->m_szUserName) )
        {
            pLoginAck->m_uLoginResult = SSC_LOGIN_ACK::USER_LOGINED;
        }
    }

    SendData(m_uConnIndex, *pLoginAck, SC_LOGIN_ACK);

    if ( pLoginAck->m_uLoginResult == SSC_LOGIN_ACK::LOGIN_OK )
    {
        m_strUserName = pLoginAck->m_szUserName;
        OnLoginOK();
    }
}

void CSrvUser::On_SC_REGISTER_ACK(const void *pParam)
{
    const SSC_REGISTER_ACK *pRegisterAck = static_cast<const SSC_REGISTER_ACK*>(pParam);

    SendData(m_uConnIndex, *pRegisterAck, SC_REGISTER_ACK);
}

void CSrvUser::On_SC_PUSH_MSG(SEventBuffer *pEventBuffer)
{
    const SSC_PUSH_MSG* pPushMsg =
            static_cast<const SSC_PUSH_MSG*>(pEventBuffer->GetEventParamPtr ());

    SendData(m_uConnIndex, *pPushMsg, SC_PUSH_MSG);
}

void CSrvUser::On_SC_TAKESITE_ACK(SEventBuffer *pEventBuffer)
{
    SSC_TAKESITE_ACK *pTakeSiteAck = static_cast<SSC_TAKESITE_ACK*>( pEventBuffer->GetEventParamPtr ());

    SendData(m_uConnIndex, *pTakeSiteAck, SC_TAKESITE_ACK);
}

void CSrvUser::On_SC_MOVE_PIECE_ACK(SEventBuffer *pEventBuffer)
{
    SSC_MOVE_PIECE_ACK* pMovePiece = static_cast<SSC_MOVE_PIECE_ACK*>( pEventBuffer->GetEventParamPtr () );

    SendData(m_uConnIndex, *pMovePiece, SC_MOVE_PIECE_ACK);
}

void CSrvUser::On_SC_PUSH_CHESS_PANEL(SEventBuffer *pEventBuffer)
{
    SSC_PUSH_CHESS_PANEL *pChessPanel =
            static_cast<SSC_PUSH_CHESS_PANEL*>(pEventBuffer->GetEventParamPtr ());

    SendData(m_uConnIndex, *pChessPanel, SC_PUSH_CHESS_PANEL);
}

void CSrvUser::On_SC_PUSH_GAME_OVER(SEventBuffer *pEventBuffer)
{
    SSC_PUSH_GAME_OVER* pGameOver = static_cast<SSC_PUSH_GAME_OVER*>(
                pEventBuffer->GetEventParamPtr () );

    SendData(m_uConnIndex, *pGameOver, SC_PUSH_GAME_OVER);
}

void CSrvUser::On_SC_PUSH_TURN_CHANGE(SEventBuffer *pEventBuffer)
{
    SSC_PUSH_TURN_CHANGE* pTurnChange = static_cast<SSC_PUSH_TURN_CHANGE*>(
                pEventBuffer->GetEventParamPtr () );
    SendData(m_uConnIndex, *pTurnChange, SC_PUSH_TURN_CHANGE);
}

void CSrvUser::On_SC_PUSH_USER_LEAVE(SEventBuffer *pEventBuffer)
{
    SSC_PUSH_USER_LEAVE* pLeaveUser = static_cast<SSC_PUSH_USER_LEAVE*>(pEventBuffer->GetEventParamPtr ());

    SendData(m_uConnIndex, *pLeaveUser, SC_PUSH_USER_LEAVE);
}

void CSrvUser::On_SC_PUSH_JIANGJU(SEventBuffer *pEventBuffer)
{
    SSC_PUSH_JIANGJU* pJiangJu = static_cast<SSC_PUSH_JIANGJU*> ( pEventBuffer->GetEventParamPtr () );

    SendData(m_uConnIndex, *pJiangJu, SC_PUSH_JIANGJU);
}

void CSrvUser::On_CS_HEART_BEAT()
{
    m_xLastHeartTime = time(NULL);
    chess::SendData(m_uConnIndex, SC_HEART_BEAT_ACK);
}

void CSrvUser::OnLoginOK()
{
    m_bIsLogined = true;
    m_xLastHeartTime = time(NULL);
    g_pSrvUserMgr->AddLoginUser (this);

    SSC_USER_LOGINED xLoginedUsers;
    std::unordered_set<std::string> const& usetLoginedUserNames = g_pSrvUserMgr->GetLoginedUserNames ();

    xLoginedUsers.m_vecLoginUsers.assign (usetLoginedUserNames.begin (), usetLoginedUserNames.end ());
    SendData(m_uConnIndex, xLoginedUsers, SC_USER_LOGINED);

    g_pChessGame->DoEnterRoom (m_uConnIndex);

    START_LOG(RO_COUT).END_LOG();
}

}
