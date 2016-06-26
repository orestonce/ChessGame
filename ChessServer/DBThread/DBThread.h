#ifndef DBTHREAD_H
#define DBTHREAD_H

#include "RoSpace/RoTypes.h"
#include "EventBuffer/EventBufferRecycle.h"
#include "EventBuffer/EventBuffer.h"
#include "SrvUser/SrvUserMgr.h"

class MySQLConnection;

namespace chess
{
class SEventBuffer;

using namespace RoSpace;

class CDBThread
{
public:
    CDBThread();
    ~CDBThread();
    /**
     * @brief Init 从配置文件工具中读取数据库链接配置, 然后通过对应的链接信息链接到数据库
     * @param uThreadIndex 该线程的线程索引 [0, uThreadCount)
     * @return true -> 链接数据库成功, false -> 链接数据库失败
     */
    bool Init(unsigned uThreadIndex);

    /**
     * @brief ProcessEvent 处理一个数据库事件
     */
    void ProcessEvent(SEventBuffer* pEventBuffer);
private :
    void On_CS_LOGIN(SEventBuffer* pEventBuffer);
    void On_CS_REGISTER(SEventBuffer* pEventBuffer);
    void On_SD_SAVE_MSG(SEventBuffer* pEventBuffer);

    template<typename SEventStruct>
    void PutEventToUserMgr(UINT uTraceID, UINT uEventType, SEventStruct const& xEvent)
    {
        SEventBuffer* pEventBuffer = g_pEventBufferRecycle->GetInvalidEventBuffer ();

        pEventBuffer->Write (uTraceID,
                             uEventType,
                             &xEvent,
                             sizeof(xEvent));
        g_pSrvUserMgr->QueueEvent (pEventBuffer);
    }

private:
    UINT m_uThreadIndex;
    MySQLConnection *m_xConn;
};
}
#endif // DBTHREAD_H
