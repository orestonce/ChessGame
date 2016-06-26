#include "ChessGame.h"
#include <iostream>
#include "RoLog/RoLog.h"
#include <fstream>

namespace chess
{
CChessGame::CChessGame()
{
    m_bIsRuing = false;

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            m_arrPiece[iLine][ iRow ].SetChessGame (this);
        }
    }
}

void CChessGame::TakeSite(UINT uConnIndex, EPieceTeam xTeam)
{
    auto fp_TakeSite = [](UINT& uSiteUser, UINT uWantSite) -> bool {
        if ( uSiteUser )
        {
            return false;
        }
        uSiteUser = uWantSite;
        return true;
    };
    bool bTakeSiteResult = false ;

    switch ( xTeam )
    {
    case EPieceTeam::Up:
        bTakeSiteResult = fp_TakeSite(m_xPlayers.m_uUp, uConnIndex);
        break;
    case EPieceTeam::Down:
        bTakeSiteResult = fp_TakeSite(m_xPlayers.m_uDown, uConnIndex);
        break;
    default :
        break;
    }

    if ( !bTakeSiteResult )
    {
        Net_PushTakeSiteResult (uConnIndex, xTeam, ETakeSiteResult::SiteHasUser);
    }
    else
    {
        OnTakeSiteOK(uConnIndex, xTeam);
    }

    if ( m_xPlayers.IsAllOK() )
    {
        OnGameStarted();
    }
}

void CChessGame::DoEnterRoom(UINT uConnIndex)
{
    if ( m_bIsRuing )
    {
        Net_PushTakeSiteResult (uConnIndex, EPieceTeam::None, ETakeSiteResult::GameIsRuning);
        Net_PushChessPanel (uConnIndex);
        Net_PushTurnChange (m_xWhoTurn, uConnIndex);
    }
    else if ( m_xPlayers.m_uDown== 0 )
    {
        TakeSite (uConnIndex, EPieceTeam::Down);
    }
    else
    {
        TakeSite (uConnIndex, EPieceTeam::Up);
    }
}

void CChessGame::DoLeave(UINT uConnIndex)
{
    Net_PushUserLeaveInfo(uConnIndex);

    if ( m_xPlayers.m_uDown == uConnIndex )
    {
        if ( m_bIsRuing )
        {
            OnGameOver(EPieceTeam::Up, EWinReason::Another_Leave);
        }
        m_xPlayers.m_uDown = 0;
    }
    else if ( m_xPlayers.m_uUp == uConnIndex )
    {
        if ( m_bIsRuing )
        {
            OnGameOver(EPieceTeam::Down, EWinReason::Another_Leave);
        }
        m_xPlayers.m_uUp = 0;
    }
    else
    {
        START_LOG(RO_COUT).OUT_LOG("Not player User leave : ").BUILD_LOG(uConnIndex).END_LOG();
    }
}

void CChessGame::DoMovePiece(UINT uConnIndex, const CPoint &xFrom, const CPoint &xTo)
{
    if ( uConnIndex == m_xPlayers.m_uUp )
    {
        MovePiece (uConnIndex, EPieceTeam::Up, xFrom, xTo);
    }
    else if ( uConnIndex == m_xPlayers.m_uDown )
    {
        MovePiece (uConnIndex, EPieceTeam::Down, xFrom, xTo);
    }
    else
    {
        START_LOG(RO_CERR).OUT_LOG("You have not permission : ").BUILD_LOG(uConnIndex).END_LOG();
    }
}

void CChessGame::DoRegame(UINT uConnIndex)
{
    if ( m_bIsRuing || m_xPlayers.IsPlayer (uConnIndex) == false)
    {
        return ;
    }

    OnGameStarted ();
}

void CChessGame::OnGameStarted()
{
    m_bIsRuing = true;
    m_xWhoTurn = EPieceTeam::Down;

    ResetChessPanel ();

    Net_PushChessPanel();
    Net_PushTurnChange (m_xWhoTurn);
}

void CChessGame::OnTakeSiteOK(UINT uConnIndex, EPieceTeam xTeam)
{
    if ( !uConnIndex )
    {
        START_LOG(RO_CERR).BUILD_LOG(uConnIndex).END_LOG();
        return ;
    }

    Net_PushTakeSiteResult (uConnIndex, xTeam, ETakeSiteResult::TakeSiteOK);
    START_LOG(RO_COUT).END_LOG();
}

void CChessGame::OnGameOver(EPieceTeam xWinTeam, EWinReason xWinReason)
{
    m_bIsRuing = false;
    Net_PushGameOver (xWinTeam, xWinReason);
}

void CChessGame::MovePiece(UINT uConnIndex, EPieceTeam xWho, const CPoint &xFrom, const CPoint &xTo)
{
    if ( !m_bIsRuing )
    {
        Net_PushMovePieceResult ( uConnIndex, xFrom, xTo, EMoveErrorCode::GameIsStoped);
        return ;
    }

    if ( xWho != m_xWhoTurn )
    {
        Net_PushMovePieceResult ( uConnIndex, xFrom, xTo, EMoveErrorCode::NotYouTurn);
        return ;
    }

    EMoveErrorCode ec = GetPiece (xFrom)->CanMove (xWho, xFrom, xTo);

    if ( ec == EMoveErrorCode::CanMove )
    {
        EPieceTeam xNextOpTeam = GetNextOpratorTeam( m_xWhoTurn );
        CPiece* pDestPiece = GetPiece (xTo);
        //        CPiece* pSourcePiece = GetPiece ( xFrom);
        bool bKillAnother_GameOver = (pDestPiece->GetType () == EPieceType::Jiang);

        SavePanel (xFrom, xTo);
        // 尝试移动棋子

        *pDestPiece = *GetPiece (xFrom);
        GetPiece (xFrom)->Clear ();
        // 如果你走了这一步就会被将军
        if ( !bKillAnother_GameOver && CanKillAnother_OneStep ( xNextOpTeam ) )
        {
            // 恢复以前的状态
            RecoverPanel (xFrom, xTo);
            // 发送移动棋子错误的消息
            Net_PushMovePieceResult (uConnIndex, xFrom, xTo, EMoveErrorCode::CanMove_But_YouWillDie);

            return ;
        }

        Net_PushMovePieceResult ( uConnIndex, xFrom, xTo, EMoveErrorCode::CanMove );


        if ( bKillAnother_GameOver )
        {
            OnGameOver(m_xWhoTurn, EWinReason::KilledJiang);
        }
        else if ( HasNoWay( xNextOpTeam ) )
        {
            OnGameOver (m_xWhoTurn, EWinReason::Another_NoWay);
        }
        else
        {
            Net_PushTurnChange(xNextOpTeam);
            if ( CanKillAnother_OneStep (m_xWhoTurn) )
            {
                Net_PushJiangJuInfo(xNextOpTeam);
            }
        }

        m_xWhoTurn = xNextOpTeam;
    }
    else
    {
        Net_PushMovePieceResult ( uConnIndex, xFrom, xTo, ec);
    }
}

bool CChessGame::IsValidPoint(const CPoint &xPoint)
{
    return xPoint.GetLine () >=0 && xPoint.GetLine () < LINE_END &&
            xPoint.GetRow () >= 0 && xPoint.GetRow () < ROW_END;
}

CPiece *CChessGame::GetPiece(const CPoint &xPoint)
{
    if ( IsValidPoint ( xPoint) )
    {
        return &m_arrPiece[ xPoint.GetLine () ] [ xPoint.GetRow () ];
    }

    return nullptr;
}

void CChessGame::Print(std::ostream& os) const
{
    os << std::boolalpha << m_bIsRuing << " : ";
    switch ( m_xWhoTurn )
    {
    case EPieceTeam::Up:
        os << "[U]" << m_xPlayers.m_uUp;
        break;
    case EPieceTeam::Down:
        os << "[D]" << m_xPlayers.m_uDown;
        break;
    default:
        break;
    }
    os << std::endl;

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            switch (m_arrPiece[iLine][iRow].GetTeam () )
            {
            case EPieceTeam::Down:
                os << "[D";
                break;
            case EPieceTeam::Up:
                os << "[U";
                break;
            default:
                os << "[ ";
                break;
            }

            switch ( m_arrPiece[iLine][iRow].GetType () )
            {
            case EPieceType::Bing:
                os << "B]";
                break;
            case EPieceType::Pao:
                os << "P]";
                break;
            case EPieceType::Che:
                os << "C]";
                break;
            case EPieceType::Ma:
                os << "M]";
                break;
            case EPieceType::Xiang:
                os << "X]";
                break;
            case EPieceType::Shi:
                os << "S]";
                break;
            case EPieceType::Jiang:
                os << "J]";
                break;
            default :
                os << " ]";
                break;
            }
        }
        os << std::endl;
    }

    os << std::endl;
}

void CChessGame::ClearPiecePanel()
{
    for(int line=0; line<LINE_END; ++line)
    {
        for(int row=0; row<ROW_END; ++row)
        {
            m_arrPiece[line][row].Clear ();
        }
    }
}

EPieceTeam CChessGame::GetNextOpratorTeam(EPieceTeam xTeam)
{
    switch ( xTeam )
    {
    case EPieceTeam::Down:
        return EPieceTeam::Up;
        break;
    case EPieceTeam::Up:
        return EPieceTeam::Down;
        break;
    default:
        break;
    }

    return EPieceTeam::None;
}

bool CChessGame::CanKillAnother_OneStep(EPieceTeam xTeam)
{
    CPoint xAnother_Jiang(0,0);
    EPieceTeam xAnotherTeam = GetNextOpratorTeam (xTeam);
    bool bFindJiangPos = false;

    // 查找对手的将所在位置
    for(int iLine=0; iLine<LINE_END ; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            CPiece* pPiece = GetPiece ( CPoint(iLine, iRow) );

            if ( pPiece->GetTeam () == xAnotherTeam &&
                 pPiece->GetType () == EPieceType::Jiang)
            {
                xAnother_Jiang.SetLine (iLine);
                xAnother_Jiang.SetRow (iRow);

                bFindJiangPos = true;
                break;
            }
        }

        if ( bFindJiangPos )
        {
            break;
        }
    }

    if ( !bFindJiangPos )
    {
        return false;
    }

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            CPiece *pPiece = GetPiece (CPoint(iLine, iRow));
            if ( pPiece->GetTeam () == xTeam )
            {
                if ( pPiece->CanMove (xTeam, CPoint(iLine, iRow), xAnother_Jiang) == EMoveErrorCode::CanMove )
                {
                    START_LOG(RO_COUT).BUILD_LOG(iLine).BUILD_LOG(iRow).END_LOG();
                    START_LOG(RO_COUT).BUILD_LOG(xAnother_Jiang.GetLine ())
                            .BUILD_LOG(xAnother_Jiang.GetRow ()).END_LOG();
                    std::cout << int(pPiece->GetTeam ()) << int(pPiece->GetType ()) << std::endl;
                    return true;
                }
            }
        }
    }

    return false;
}

void CChessGame::ResetChessPanel()
{
    ClearPiecePanel ();

    EPieceTeam xTeam = EPieceTeam::Up;

    m_arrPiece[0][0].Reset (xTeam, EPieceType::Che );
    m_arrPiece[0][1].Reset (xTeam, EPieceType::Ma);
    m_arrPiece[0][2].Reset (xTeam, EPieceType::Xiang);
    m_arrPiece[0][3].Reset (xTeam, EPieceType::Shi);
    m_arrPiece[0][4].Reset (xTeam, EPieceType::Jiang);
    m_arrPiece[0][5].Reset (xTeam, EPieceType::Shi);
    m_arrPiece[0][6].Reset (xTeam, EPieceType::Xiang);
    m_arrPiece[0][7].Reset (xTeam, EPieceType::Ma);
    m_arrPiece[0][8].Reset (xTeam, EPieceType::Che);

    m_arrPiece[2][1].Reset (xTeam, EPieceType::Pao);
    m_arrPiece[2][7].Reset (xTeam, EPieceType::Pao);

    m_arrPiece[3][0].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[3][2].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[3][4].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[3][6].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[3][8].Reset (xTeam, EPieceType::Bing);

    xTeam = EPieceTeam::Down;

    m_arrPiece[9][0].Reset (xTeam, EPieceType::Che );
    m_arrPiece[9][1].Reset (xTeam, EPieceType::Ma);
    m_arrPiece[9][2].Reset (xTeam, EPieceType::Xiang);
    m_arrPiece[9][3].Reset (xTeam, EPieceType::Shi);
    m_arrPiece[9][4].Reset (xTeam, EPieceType::Jiang);
    m_arrPiece[9][5].Reset (xTeam, EPieceType::Shi);
    m_arrPiece[9][6].Reset (xTeam, EPieceType::Xiang);
    m_arrPiece[9][7].Reset (xTeam, EPieceType::Ma);
    m_arrPiece[9][8].Reset (xTeam, EPieceType::Che);

    m_arrPiece[7][1].Reset (xTeam, EPieceType::Pao);
    m_arrPiece[7][7].Reset (xTeam, EPieceType::Pao);

    m_arrPiece[6][0].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[6][2].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[6][4].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[6][6].Reset (xTeam, EPieceType::Bing);
    m_arrPiece[6][8].Reset (xTeam, EPieceType::Bing);
}

bool CChessGame::HasNoWay(EPieceTeam xTeam)
{
    EPieceTeam xAnotherTeam = GetNextOpratorTeam (xTeam);

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<ROW_END; ++iRow)
        {
            CPoint xFrom(iLine, iRow);
            CPiece* pFromPiece = GetPiece (xFrom);

            if ( pFromPiece->GetTeam () != xTeam )
            {
                continue;
            }

            for(int iLine=0; iLine<LINE_END; ++iLine)
            {
                for(int iRow=0; iRow<ROW_END; ++iRow)
                {
                    CPoint xTo(iLine, iRow);
                    if ( pFromPiece->CanMove (xTeam, xFrom, xTo) == EMoveErrorCode::CanMove )
                    {
                        // 我走一步后, 别人不能在一步之内杀死我, 说明我还有棋子可以走
                        SavePanel (xFrom, xTo);
                        m_arrPiece[xTo.GetLine ()][xTo.GetRow ()] = m_arrPiece[xFrom.GetLine ()][xFrom.GetRow ()];
                        m_arrPiece[xFrom.GetLine ()][xFrom.GetRow ()].Clear ();

                        if ( !CanKillAnother_OneStep (xAnotherTeam) )
                        {
                            RecoverPanel (xFrom, xTo);
                            return false;
                        }
                        RecoverPanel (xFrom, xTo);
                    }
                }
            }
        }
    }

    return true;
}

}
