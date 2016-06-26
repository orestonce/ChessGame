#include "ChessGame.h"
#include <iostream>
#include "RoLog/RoLog.h"
#include "Protol/CSProtol.h"
#include "Protol/CSProtol.SendData.hpp"
#include "EventBuffer/EventBuffer.h"
#include "EventBuffer/EventBufferRecycle.h"
#include "SrvUser/SrvUserMgr.h"
#include "SrvUser/SrvUser.h"

#define GET_USERNAME( uConnIndex ) g_pSrvUserMgr->GetValidUserPtr (uConnIndex)->GetUserName ().c_str ()

namespace chess
{

void CChessGame::Net_PushTakeSiteResult(UINT uOprateUser, EPieceTeam xTeam, ETakeSiteResult xResult)
{
    SEventBuffer *pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();
    struct SSC_TAKESITE_ACK xTakeSiteACK;

    strncpy (xTakeSiteACK.m_szUserName, g_pSrvUserMgr->GetUserPtr (uOprateUser)->GetUserName ().c_str (),
             sizeof( xTakeSiteACK.m_szUserName ) );
    xTakeSiteACK.m_uDestSite = static_cast<UINT>(xTeam);
    xTakeSiteACK.m_uTakeSiteResult = static_cast<UINT>( xResult );

    if ( xResult == ETakeSiteResult::TakeSiteOK )
    {
        uOprateUser = 0;
    }

    pEventBuffer->Write (uOprateUser, SC_TAKESITE_ACK, &xTakeSiteACK, sizeof( xTakeSiteACK) );

    g_pSrvUserMgr->QueueEvent (pEventBuffer);
}

void CChessGame::Net_PushChessPanel(UINT uDest)
{
    SEventBuffer *pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();
    SSC_PUSH_CHESS_PANEL xChessPanel;
    auto& xPanel = xChessPanel.m_arr;

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            CPiece* pPiece = &m_arrPiece[iLine][iRow];

            xPanel[iLine][iRow].m_uChessTeam = static_cast<USHORT> (pPiece->GetTeam ());
            xPanel[iLine][iRow].m_uChessType  = static_cast<USHORT> (pPiece->GetType  ());
            xPanel[iLine][iRow].m_xPoint.m_iLine = iLine;
            xPanel[iLine][iRow].m_xPoint.m_iRow = iRow;
        }
    }
    xChessPanel.m_bIsRuning = m_bIsRuing;

    strncpy (xChessPanel.m_szUpUserName, GET_USERNAME(m_xPlayers.m_uUp),
             sizeof(xChessPanel.m_szUpUserName));
    strncpy (xChessPanel.m_szDownUserName, GET_USERNAME(m_xPlayers.m_uDown),
             sizeof(xChessPanel.m_szDownUserName));

    pEventBuffer->Write (uDest, SC_PUSH_CHESS_PANEL, &xChessPanel, sizeof(xChessPanel) );

    g_pSrvUserMgr->QueueEvent (pEventBuffer);

    Print ();
}

void CChessGame::Net_PushMovePieceResult(UINT uConnIndex, CPoint const& xFrom,
                                         CPoint const& xTo, EMoveErrorCode xError)
{
    SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();
    struct SSC_MOVE_PIECE_ACK xMovePieceAck;

    strncpy (xMovePieceAck.m_szUserName, g_pSrvUserMgr->GetUserPtr (uConnIndex)->GetUserName ().c_str (),
             sizeof( xMovePieceAck.m_szUserName) );
    xMovePieceAck.m_uMoveResult = static_cast<UINT>(xError);
    xMovePieceAck.m_xFrom.m_iLine = xFrom.GetLine ();
    xMovePieceAck.m_xFrom.m_iRow = xFrom.GetRow ();

    xMovePieceAck.m_xTo.m_iLine = xTo.GetLine ();
    xMovePieceAck.m_xTo.m_iRow = xTo.GetRow ();

    if ( xError  == EMoveErrorCode::CanMove )
    {
        uConnIndex = 0;
    }

    pEventBuffer->Write (uConnIndex, SC_MOVE_PIECE_ACK, &xMovePieceAck, sizeof(xMovePieceAck) );
    g_pSrvUserMgr->QueueEvent ( pEventBuffer );
}

void CChessGame::Net_PushGameOver(EPieceTeam xWinTeam, EWinReason xWinReason)
{
    SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

    SSC_PUSH_GAME_OVER xGameOver ;

    xGameOver.m_uWinnerTeam = static_cast<UINT>( xWinTeam);
    xGameOver.m_uWinReason   = static_cast<UINT>( xWinReason );

    pEventBuffer->Write (0, SC_PUSH_GAME_OVER, &xGameOver, sizeof(xGameOver) );

    g_pSrvUserMgr->QueueEvent (pEventBuffer);

    START_LOG(RO_COUT).END_LOG();
}

void CChessGame::Net_PushTurnChange(EPieceTeam xTeam, UINT uPushTo)
{
    UINT uPlayerIndex = 0;

    switch (xTeam) {
    case EPieceTeam::Up:
        uPlayerIndex = m_xPlayers.m_uUp;
        break;
    case EPieceTeam::Down:
        uPlayerIndex = m_xPlayers.m_uDown;
        break;
    default:
        START_LOG(RO_CERR).END_LOG();
        return ;
        break;
    }

    SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();
    struct SSC_PUSH_TURN_CHANGE xTurnChange;

    strncpy (xTurnChange.m_szTurnNow,
             GET_USERNAME(uPlayerIndex), sizeof(xTurnChange) );

    pEventBuffer->Write ( uPushTo, SC_PUSH_TURN_CHANGE, &xTurnChange, sizeof(xTurnChange) );
    g_pSrvUserMgr->QueueEvent (pEventBuffer);

    std::cout << "Turn : " << static_cast<int>( xTeam) << std::endl;
}

void CChessGame::Net_PushUserLeaveInfo(UINT uConnIndex)
{
    SSC_PUSH_USER_LEAVE xLeaveUser;
    strncpy (xLeaveUser.m_szLeaveUserName, GET_USERNAME(uConnIndex), sizeof(xLeaveUser.m_szLeaveUserName));

    SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

    pEventBuffer->Write (0, SC_PUSH_USER_LEAVE, &xLeaveUser, sizeof(xLeaveUser));
    g_pSrvUserMgr->QueueEvent (pEventBuffer);

    START_LOG(RO_COUT).EndLog ();
}

void CChessGame::Net_PushJiangJuInfo(EPieceTeam xBeiJiangJunTeam)
{
    std::cout << "BeiJiangJun : " << static_cast<int>( xBeiJiangJunTeam ) << std::endl;

    SSC_PUSH_JIANGJU xJiangJu;

    UINT uPlayerIndex = 0;

    switch (xBeiJiangJunTeam) {
    case EPieceTeam::Up:
        uPlayerIndex = m_xPlayers.m_uUp;
        break;
    case EPieceTeam::Down:
        uPlayerIndex = m_xPlayers.m_uDown;
        break;
    default:
        START_LOG(RO_CERR).END_LOG();
        return ;
        break;
    }

    strncpy (xJiangJu.m_szBeiJiangJuUserName, GET_USERNAME(uPlayerIndex), sizeof(xJiangJu) );


    SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

    pEventBuffer->Write (0, SC_PUSH_JIANGJU, &xJiangJu, sizeof( xJiangJu) );
    g_pSrvUserMgr->QueueEvent (pEventBuffer);

    START_LOG(RO_COUT).EndLog ();
}

void CChessGame::SavePanel(const CPoint &xFrom, const CPoint &xTo)
{
    m_xOldPanel.m_xSource = m_arrPiece[xFrom.GetLine ()][xFrom.GetRow ()];
    m_xOldPanel.m_xDest     = m_arrPiece[xTo.GetLine ()][xTo.GetRow ()];
}

void CChessGame::RecoverPanel (const CPoint &xFrom, const CPoint &xTo)
{
    m_arrPiece[xFrom.GetLine ()][xFrom.GetRow ()] = m_xOldPanel.m_xSource;
    m_arrPiece[xTo.GetLine ()][xTo.GetRow ()] = m_xOldPanel.m_xDest;
}

}
