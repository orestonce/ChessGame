#include "Piece.h"
#include "ChessGame/ChessGame.h"
#include "RoLog/RoLog.h"
#include <math.h>

namespace chess
{

CPiece::CPiece()
{
    Clear ();
}

void CPiece::SetChessGame(CChessGame *pChessGame)
{
    m_pChessGame = pChessGame;
}

void CPiece::Reset(EPieceTeam ePieceTeam, EPieceType ePieceType)
{
    m_xTeam = ePieceTeam;
    m_xType  = ePieceType;

    switch (m_xType) {
    case EPieceType::Bing:
        m_fpCanMove = &CPiece::_CanMove_Bing ;
        break;
    case EPieceType::Che:
        m_fpCanMove = &CPiece::_CanMove_Che ;
        break;
    case EPieceType::Jiang:
        m_fpCanMove = &CPiece::_CanMove_Jiang ;
        break;
    case EPieceType::Ma:
        m_fpCanMove = &CPiece::_CanMove_Ma ;
        break;
    case EPieceType::Pao:
        m_fpCanMove = &CPiece::_CanMove_Pao ;
        break;
    case EPieceType::Shi:
        m_fpCanMove = &CPiece::_CanMove_Shi ;
        break;
    case EPieceType::Xiang :
        m_fpCanMove = &CPiece::_CanMove_Xiang ;
        break;
    default:
        m_fpCanMove = nullptr;
        break;
    }
}

void CPiece::Clear()
{
    Reset (EPieceTeam::None, EPieceType::None);
}

bool CPiece::IsNone() const
{
    return m_fpCanMove == nullptr;
}


/**
enum EMoveErrorCode
{
    CanMove,                                // [Piece] 能够移动
    GameIsStoped,                         // [Game]游戏已停止
    NotYouTurn,                            // [Game]不该你走
    SourceIsNotYours,                     // [Piece]起点地址不是你的棋子
    DestnationIsYours,                    // [Piece]终点位置是你的棋子
    CanMove_But_YouWillDie,        // [Game]这样移动后你就会被一步KILL
    NoMatchPieceRule                     // [Piece]移动不符合规则
};
*/
EMoveErrorCode CPiece::CanMove(EPieceTeam eOpraterTeam, const CPoint &xFrom, const CPoint &xTo)
{
    CPiece* pSourcePiece = m_pChessGame->GetPiece (xFrom);

    if ( pSourcePiece != this ||
         pSourcePiece->GetTeam () == EPieceTeam::None ||
         m_xTeam != eOpraterTeam )
    {
        return EMoveErrorCode::SourceIsNotYours;
    }

    CPiece* pDestPiece = m_pChessGame->GetPiece (xTo);

    if ( !pDestPiece || pDestPiece->GetTeam () == m_xTeam )
    {
        return EMoveErrorCode::DestnationIsYours;
    }

    if ( (this->*m_fpCanMove)(xFrom, xTo) )
    {
        if ( xTo == CPoint(9, 4) )
        {
            START_LOG(RO_COUT)
                    .BUILD_LOG(xFrom.GetLine ())
                    .BUILD_LOG(xFrom.GetRow ()).END_LOG();
        }

        return EMoveErrorCode::CanMove;
    }

    return EMoveErrorCode::NoMatchPieceRule;
}

EPieceTeam CPiece::GetTeam() const
{
    return m_xTeam;
}

EPieceType CPiece::GetType() const
{
    return m_xType;
}

}
