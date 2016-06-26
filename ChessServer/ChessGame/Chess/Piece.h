#ifndef PIECE_H
#define PIECE_H


#include "Point.h"
#include "Protol/Common.h"

namespace chess
{

class CChessGame;

class CPiece
{
public:
    // 移动棋子算法指针
    typedef bool (CPiece:: *FP_CanMove )( CPoint const& xFrom, CPoint const& xTo );

    CPiece();

    // 设置象棋游戏指针
    void SetChessGame(CChessGame *pChessGame);
    /**
     * @brief Reset 重置棋子的类型和队伍
     * @param ePieceTeam
     * @param ePieceType
     */
    void Reset(EPieceTeam ePieceTeam,
               EPieceType ePieceType);

    /**
     * @brief Clear 清空棋子信息, 清空过后 IsNone函数返回 true
     */
    void Clear();

    /**
     * @brief IsNone 判断棋子是否存在
     * @return true -> 棋子不存在, false -> 棋子存在
     */
    bool IsNone() const;
    /**
     * @brief CanMove   判断从一个位置移动到另一个位置是不是符合移动规则
     * @param eOpraterTeam 操作队伍
     * @param xFrom 移动起点
     * @param xTo 移动终点
     * @return 能否移动的状态信息
     */
    EMoveErrorCode CanMove(EPieceTeam eOpraterTeam, CPoint const& xFrom, CPoint const& xTo);

    /**
     * @brief GetTeam 获取棋子队伍信息
     * @return 棋子队伍信息
     */
    EPieceTeam GetTeam() const;
    /**
     * @brief GetType 获取棋子类型信息
     * @return 棋子类型信息
     */
    EPieceType  GetType() const;
private:
    /*---------一系列的运算规则-----------*/
    bool _CanMove_Che (CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Ma (CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Xiang(CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Shi(CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Jiang(CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Pao(CPoint const& xFrom, CPoint const& xTo);
    bool _CanMove_Bing(CPoint const& xFrom, CPoint const& xTo);
private:
    EPieceType     m_xType;  //  棋子类型
    EPieceTeam    m_xTeam; // 棋子队伍
    FP_CanMove   m_fpCanMove; // 移动棋子需要调用的判断函数
    CChessGame* m_pChessGame; // 棋子所属的棋盘
};

}

#endif // PIECE_H
