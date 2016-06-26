#ifndef EVENTBUFFERRECYCLE_H
#define EVENTBUFFERRECYCLE_H

#include <mutex>
#include <queue>
#include "RoSpace/RoSingleton.h"

namespace chess
{

struct SEventBuffer;

class CEventBufferRecycle
        : public RoSpace::CSingleton<CEventBufferRecycle>
{
public:
    ~CEventBufferRecycle();
    /**
     * @brief GetInvalidEventBuffer 获取一个事件缓冲区, 优先从回收站中找, 没有就new
     * @return 获取到的事件缓冲区
     */
    SEventBuffer *GetInvalidEventBuffer();
    /**
     * @brief RecycleEventBuffer 回收事件缓冲区
     */
    void RecycleEventBuffer(SEventBuffer* pEventBuffer);
private:
    std::mutex m_mtxSyncRecycle;
    std::queue<SEventBuffer*> m_queRecycle;
};

#define g_pEventBufferRecycle chess::CEventBufferRecycle::Instance ()

}
#endif // EVENTBUFFERRECYCLE_H
