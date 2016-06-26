#ifndef CHESSSERVER_H
#define CHESSSERVER_H

#include "RoSpace/RoSingleton.h"
#include "RoSpace/RoTypes.h"

namespace chess
{
using namespace RoSpace;

class CChessServer
        : public RoSpace::CSingleton<CChessServer>
{
public:
    CChessServer();
    /**
     * @brief StartRun 开始运行象棋服务器
     */
    void StartRun();

    /**
     * @brief SetQuitFlag 设置退出标志为true
     */
    void SetQuitFlag();
private:
    /**
     * @brief SetupSignals 初始化信号处理函数
     */
    void SetupSignals();
    /**
     * @brief InitAll 初始化所有
     * @return true -> 初始化成功, false -> 初始化失败
     */
    bool InitAll();
    /**
     * @brief FreeAll 释放所有
     */
    void FreeAll();
    /**
     * @brief LogicTick 进入业务逻辑处理
     */
    void LogicTick();

    /**
     * @brief InitNetworkLib 初始化网络库
     * @return true-> 初始化成功, false -> 初始化失败
     */
    bool InitNetworkLib();
private:
    /*-----网络库回调函数-------*/
    static void _AppLogic();
    static void _OnAcceptOK(UINT uConnIndex);
    static void _OnRecvData(UINT uConnIndex, const char* pBuffer, UINT uBufferBytes);
    static void _OnDisconnected(UINT uConnIndex);
    /**
     * @brief _OnSignal 信号被触发后的回调函数
     */
    static void _OnSignal(int );
private:
    volatile bool m_bQuitFlag; // 退出标志
};

#define g_pChessServer chess::CChessServer::Instance ()

}
#endif // CHESSSERVER_H
