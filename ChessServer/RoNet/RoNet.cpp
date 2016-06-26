#include "RoNet.h"
#include "RoNet/private/RoNetMgr.h"
#include "RoLog/RoLog.h"

namespace RoNet
{

bool InitNetworkLib (const SRoNetInitData &xData)
{
    return g_pRoNetMgr->Init (xData);
}

void FreeNetworkLib ()
{
    g_pRoNetMgr->Free();
}

bool LogicTick()
{
    return g_pRoNetMgr->LogicTick();
}

void Disconnect(UINT uConnIndex)
{
    g_pRoNetMgr->Disconnect( uConnIndex );
}

bool SendData(UINT uConnIndex, const char *pBuffer, UINT uBufferBytes)
{
    return g_pRoNetMgr->SendData(uConnIndex, pBuffer, uBufferBytes);
}

}
