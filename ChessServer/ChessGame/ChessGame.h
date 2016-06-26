#ifndef CHESSGAME_H
#define CHESSGAME_H

#include "RoSpace/RoSingleton.h"
#include "RoSpace/RoTypes.h"
#include <array>
#include "Chess/Piece.h"
#include <map>
#include <set>
#include <iostream>

namespace chess
{
using namespace RoSpace;

class CChessGame
        : public RoSpace::CSingleton<CChessGame>
{
public:
    enum {
        LINE_END = 10,
        ROW_END = 9
    };
    CChessGame();
    CPiece *GetPiece(const CPoint& xPoint);

    /*----------------玩家操作------------*/
    void DoEnterRoom(UINT uConnIndex);
    void DoLeave(UINT uConnIndex);

    void DoMovePiece (UINT uConnIndex, const CPoint &xFrom, const CPoint &xTo);

    void DoRegame(UINT uConnIndex);
private:
    void TakeSite(UINT uConnIndex, EPieceTeam xTeam);
    void MovePiece(UINT uConnIndex, EPieceTeam xWho, const CPoint& xFrom, const CPoint& xTo);

    static bool IsValidPoint(const CPoint& xPoint) ;
    std::map<CPiece const*, CPoint> GetAlivePieces() const;
    EPieceTeam GetOpratorTeam() const;
private: // events
    void OnGameStarted();
    void OnTakeSiteOK(UINT uConnIndex, EPieceTeam xTeam);
    void OnGameOver(EPieceTeam xWinTeam, EWinReason xWinReason);

private: // tools
    void Print(std::ostream &os = std::cout) const;

    void ClearPiecePanel();
    EPieceTeam GetNextOpratorTeam(EPieceTeam xTeam);
    bool CanKillAnother_OneStep(EPieceTeam xTeam);
    void ResetChessPanel();
    bool HasNoWay(EPieceTeam xTeam);

private: // net-events
    /**
     * @brief Net_PushChessPanel 推送棋盘信息和棋盘状态给所有用户[2个玩家]
     */
    void Net_PushChessPanel(UINT uDest=0);
    /**
     * @brief Net_PushTakeSiteOK 推送坐下成功的消息给所有用户
     * @param uOprateUser 请求坐下的用户
     * @param xTeam 坐下的位置
     * @param xResult 坐下的结果
     */
    void Net_PushTakeSiteResult(UINT uOprateUser, EPieceTeam xTeam, ETakeSiteResult xResult);

    /**
     * @brief Net_PushMovePieceResult 推送移动棋子信息
     * @param uConnIndex 移动棋子的人
     * @param xFrom 棋子起点
     * @param xTo 棋子终点
     * @param xError 错误信息
     */
    void Net_PushMovePieceResult(UINT uConnIndex, CPoint const& xFrom, CPoint const& xTo, EMoveErrorCode xError);

    /**
     * @brief Net_PushGameOver 推送游戏结束的消息给所有用户
     * @param xWinTeam 赢了的队伍
     * @param xWinReason 赢的原因
     */
    void Net_PushGameOver(EPieceTeam xWinTeam, EWinReason xWinReason);
    /**
     * @brief Net_PushTurnChange 推送行棋状态给所有用户
     * @param xTeam 更新后的行棋队伍
     */
    void Net_PushTurnChange(EPieceTeam xTeam, UINT uPushTo=0);
    /**
     * @brief Net_PushUserLeaveInfo 推送用户断开的消息给所有用户
     * @param uConnIndex 断开的链接索引
     */
    void Net_PushUserLeaveInfo(UINT uConnIndex);

    /**
     * @brief Net_PushJiangJuInfo 推送将军信息给所有用户
     * @param xBeiJiangJunTeam 被将军队伍
     */
    void Net_PushJiangJuInfo(EPieceTeam xBeiJiangJunTeam);
private:
    /**
     * @brief The SPlayers struct 一个棋盘上面的2个玩家的结构体
     */
    struct SPlayers
    {
        UINT m_uUp;
        UINT m_uDown;
    public :
        SPlayers()
        {
            m_uDown = m_uUp = 0;
        }
        /**
         * @brief IsAllOK 所有玩家都准备好了
         * @return true -> 都准备好了, false -> 至少一个没有准备好
         */
        bool IsAllOK()
        {
            return m_uDown != 0 && m_uUp != 0;
        }
        /**
         * @brief IsPlayer 判断索引是不是一个玩家的索引
         * @param uConnIndex 需要判断的索引
         * @return tru -> 是一个玩家, false -> 不是玩家
         */
        bool IsPlayer(UINT uConnIndex)
        {
            return m_uDown == uConnIndex || m_uUp == uConnIndex;
        }
    };

    struct SSavedPanel
    {
        CPiece m_xSource;
        CPiece m_xDest;
    };
private:
    void SavePanel(CPoint const& xFrom, CPoint const& xTo);
    void RecoverPanel(CPoint const& xFrom, CPoint const& xTo);
private:

    SPlayers m_xPlayers; // 两个玩家
    bool m_bIsRuing; // 游戏是不是正在运行

    EPieceTeam m_xWhoTurn; // 现在该谁行棋
    CPiece m_arrPiece[LINE_END][ROW_END]; //  棋盘状态
    SSavedPanel m_xOldPanel; // 暂存的以前的棋盘面板状态
};

#define g_pChessGame chess::CChessGame::Instance ()
}
#endif // CHESSGAME_H
