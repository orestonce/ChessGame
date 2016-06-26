#include "ChessServer.h"
#include "RoNet/RoNet.h"
#include "SrvConfig/CSrvConfig.h"
#include "SrvUser/SrvUserMgr.h"
#include "SrvUser/SrvUser.h"
#include "DBThread/DBThreadMgr.h"
#include <sys/signal.h>

namespace chess
{

CChessServer::CChessServer()
{
    m_bQuitFlag = false;
}

void CChessServer::StartRun()
{
    SetupSignals();

    if ( InitAll() )
    {
        LogicTick();
        FreeAll();
    }
}

void CChessServer::SetQuitFlag()
{
    m_bQuitFlag = true;
}

void CChessServer::SetupSignals()
{
    const int arrQuitSignals[]= {SIGHUP, SIGINT, SIGTERM};

    for(auto i : arrQuitSignals)
    {
        signal ( i, _OnSignal );
    }
}

bool CChessServer::InitAll()
{
    return g_pSrvConfig->Init ("./chess_config.ini") &&
            g_pSrvUserMgr->Init ( g_pSrvConfig->GetMaxUserCount () ) &&
            g_pDBThreadMgr->Init ()&&
            InitNetworkLib() ;
}

void CChessServer::FreeAll()
{
    RoNet::FreeNetworkLib ();
    g_pDBThreadMgr->JoinAll ();
}

void CChessServer::LogicTick()
{
    while ( !m_bQuitFlag )
    {
        RoNet::LogicTick ();
    }
}

bool CChessServer::InitNetworkLib()
{
    RoNet::SRoNetInitData xData;
    xData.m_uMaxConnCount = g_pSrvConfig->GetMaxUserCount ();

    xData.m_uListenPort = g_pSrvConfig->GetListenPort ();

    xData.m_fpAppLogic = _AppLogic ;
    xData.m_fpOnAcceptOK = _OnAcceptOK ;
    xData.m_fpOnDisconnected = _OnDisconnected;
    xData.m_fpOnRecvData = _OnRecvData;
    xData.m_uRecvBufferByte = g_pSrvConfig->GetRecvBufferByte ();

    return RoNet::InitNetworkLib ( xData );
}

void CChessServer::_AppLogic()
{
    g_pSrvUserMgr->LogicTick ();
}

void CChessServer::_OnAcceptOK(UINT uConnIndex)
{
    g_pSrvUserMgr->OnAcceptOK ( uConnIndex );
}

void CChessServer::_OnRecvData(UINT uConnIndex, const char *pBuffer, UINT uBufferByte)
{
    CSrvUser* pUser = g_pSrvUserMgr->GetValidUserPtr (uConnIndex);
    if ( pUser )
    {
        pUser->OnRecvData (pBuffer, uBufferByte);
    }
}

void CChessServer::_OnDisconnected(UINT uConnIndex)
{
    CSrvUser* pUser = g_pSrvUserMgr->GetValidUserPtr (uConnIndex);

    if ( pUser )
    {
        pUser->ReverseDelete ();
    }
}

void CChessServer::_OnSignal(int)
{
    g_pChessServer->SetQuitFlag ();
}

}
