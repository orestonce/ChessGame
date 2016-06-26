#include "Piece.h"
#include "Point.h"
#include <math.h>
#include <algorithm>
#include "ChessGame/ChessGame.h"

namespace chess
{

bool CPiece::_CanMove_Che(const CPoint &xFrom, const CPoint &xTo)
{
    SIncrease xInc;

    xInc.m_iLineIncrease = xInc.m_iRowIncrease = 0;

    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();

    if ( iDLine && iDRow == 0 )
    {
        xInc.m_iLineIncrease =  iDLine / abs(iDLine);
    }
    else if ( iDRow && iDLine == 0)
    {
        xInc.m_iRowIncrease = iDRow / abs( iDRow );
    }
    else
    {
        return false;
    }

    int iLoopCount = std::max( abs(iDLine), abs(iDRow) );

    for(int i=1; i<iLoopCount; ++i)
    {
        if ( !(m_pChessGame->GetPiece (xFrom + xInc * i )->IsNone () ) )
        {
            return false;
        }
    }

    return true;
}

bool CPiece::_CanMove_Ma(const CPoint &xFrom, const CPoint &xTo)
{
    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();

    // 2个属于自然数相乘, 等于2只有 [1x2、2x1]才符合要求
    if ( abs(iDLine) * abs(iDRow) != 2 )
    {
        return false;
    }

    CPoint xMustEmptyPoint(0,0 );
    // 计算蹩脚点坐标
    if ( abs(iDLine) == 2 ) // 纵向移动
    {
        xMustEmptyPoint.SetRow ( xFrom.GetRow () );
        xMustEmptyPoint.SetLine ( (xFrom.GetLine () + xTo.GetLine ())/2);
    }
    else // 横向移动
    {
        xMustEmptyPoint.SetRow ( (xFrom.GetRow () + xTo.GetRow ())/2 );
        xMustEmptyPoint.SetLine ( xFrom.GetLine () );
    }

    return m_pChessGame->GetPiece (xMustEmptyPoint)->IsNone ();
}

bool CPiece::_CanMove_Xiang(const CPoint &xFrom, const CPoint &xTo)
{
    bool bCrossRiver = true;

    switch ( m_xTeam )
    {
    case EPieceTeam::Down :
        bCrossRiver =  (xTo.GetLine () <4);
        break;
    case EPieceTeam::Up:
        bCrossRiver = (xTo.GetLine () > 4);
        break;
    default :
        break;
    }

    // 过河了
    if ( bCrossRiver )
    {
        return false;
    }

    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();

    return ( abs(iDLine) == 2  && abs(iDRow) == 2 );
}

inline bool FitRange_JiangShi(const CPoint& xPoint)
{
    return (xPoint.GetRow () >= 3 && xPoint.GetRow () <= 5) &&
            ( xPoint.GetLine () <=2 || xPoint.GetLine () >= 7);
};

bool CPiece::_CanMove_Shi(const CPoint &xFrom, const CPoint &xTo)
{
    if ( !FitRange_JiangShi (xTo) )
    {
        return false;
    }

    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();

    return abs( iDLine * iDRow ) == 1;
}

bool CPiece::_CanMove_Jiang(const CPoint &xFrom, const CPoint &xTo)
{
    if ( !FitRange_JiangShi (xTo) )
    {
        return false;
    }

    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();
    // “将” 有一个特殊走法
    //   在同一列上, 起点和终点都是 “将”, 并且起点终点之间没有多于的棋子, 起点的将能够直接到达终点的将
    if ( iDRow == 0 )
    {
        CPiece *pPiece = m_pChessGame->GetPiece (xTo);

        if ( pPiece->GetType () == EPieceType::Jiang )
        {
            bool bCenterHasPiece = false;
            int iBeginLine = std::min(xFrom.GetLine (), xTo.GetLine ());
            int iEndLine   = std::max(xFrom.GetLine (), xTo.GetLine ());

            for(int iLine=iBeginLine+1; iLine < iEndLine; ++iLine)
            {
                if ( !g_pChessGame->GetPiece (CPoint(iLine, xFrom.GetRow ()))->IsNone () )
                {
                    bCenterHasPiece = true;
                    break;
                }
            }

            if ( !bCenterHasPiece )
            {
                return true;
            }
        }
    }

    return (abs(iDLine) == 1 && abs(iDRow) == 0 ) ||
            ( abs(iDRow) == 1 && abs(iDLine) == 0);
}

bool CPiece::_CanMove_Pao(const CPoint &xFrom, const CPoint &xTo)
{

    // 炮越过一个棋子, 击中对方棋子
    if ( !m_pChessGame->GetPiece (xTo)->IsNone () )
    {
        SIncrease xInc;

        if ( xFrom.GetLine () == xTo.GetLine () )
        {
            int iDRow = xTo.GetRow () - xFrom.GetRow ();

            xInc.m_iLineIncrease = 0;
            xInc.m_iRowIncrease = iDRow / abs( iDRow );
        }
        else if ( xFrom.GetRow () == xTo.GetRow () )
        {
            int iDLine = xTo.GetLine () - xFrom.GetLine ();

            xInc.m_iLineIncrease = iDLine / abs( iDLine );
            xInc.m_iRowIncrease = 0;
        }
        else
        {
            return false;
        }

        bool bFoundOnePiece = false;

        for(CPoint xPoint = xFrom + xInc; !(xPoint == xTo); xPoint = xPoint + xInc)
        {
            if ( !m_pChessGame->GetPiece ( xPoint )->IsNone ())
            {
                if ( bFoundOnePiece )
                {
                    return false;
                }
                else
                {
                    bFoundOnePiece = true;
                }
            }
        }

        if ( bFoundOnePiece )
        {
            return true;
        }

        return false;
    }
    // 炮横向和纵向移动
    else if ( _CanMove_Che (xFrom, xTo) )
    {
        return true;
    }

    return false;
}

bool CPiece::_CanMove_Bing(const CPoint &xFrom, const CPoint &xTo)
{
    int iDLine = xTo.GetLine () - xFrom.GetLine ();
    int iDRow = xTo.GetRow () - xFrom.GetRow ();

    // 左右移动, 如果已经过河-> 可以, 否则 -> 不行
    if ( abs(iDRow) == 1 && iDLine == 0)
    {
        if ( m_xTeam == EPieceTeam::Down )
        {
            return xFrom.GetLine () <= 4;
        }
        else if ( m_xTeam == EPieceTeam::Up )
        {
            return xFrom.GetLine () >=5;
        }
    }
    // 上下移动, 不准后退
    else if ( abs(iDLine) == 1 && iDRow == 0)
    {
        if ( m_xTeam == EPieceTeam::Down )
        {
            return xFrom.GetLine () >xTo.GetLine ();
        }
        else if ( m_xTeam == EPieceTeam::Up )
        {
            return xFrom.GetLine () < xTo.GetLine ();
        }
    }

    return false;
}

}
