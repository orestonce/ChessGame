#ifndef DBTHREADMGR_H
#define DBTHREADMGR_H

#include "RoSpace/RoSingleton.h"
#include "RoSpace/RoThreadPool.h"
#include "DBThread.h"

namespace chess
{
class SEventBuffer;

class CDBThreadMgr
        : public RoSpace::CSingleton<CDBThreadMgr>,
        public RoSpace::CThreadPool<CDBThread, SEventBuffer*>
{
public:
    bool Init();
};

#define g_pDBThreadMgr chess::CDBThreadMgr::Instance ()
}

#endif // DBTHREADMGR_H
