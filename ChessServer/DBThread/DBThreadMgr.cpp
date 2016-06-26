#include "DBThreadMgr.h"
#include "SrvConfig/CSrvConfig.h"
#include "RoLog/RoLog.h"

namespace chess
{

bool CDBThreadMgr::Init ()
{
    UINT uDBThreadCount = g_pSrvConfig->GetDBThreadCount ();

    if ( 0 == uDBThreadCount )
    {
        START_LOG(RO_CERR).BUILD_LOG( uDBThreadCount ).END_LOG();
        return false;
    }

    return CThreadPool::Init (uDBThreadCount);
}

}
