#include "EventBufferRecycle.h"
#include "EventBuffer.h"

namespace chess
{

CEventBufferRecycle::~CEventBufferRecycle()
{
    std::lock_guard<std::mutex> lk( m_mtxSyncRecycle );

    while ( m_queRecycle.empty () )
    {
        delete m_queRecycle.front ();
        m_queRecycle.pop ();
    }
}

SEventBuffer *CEventBufferRecycle::GetInvalidEventBuffer()
{
    {
        std::lock_guard<std::mutex> lk(m_mtxSyncRecycle);
        if ( !m_queRecycle.empty () )
        {
            SEventBuffer* pEventBuffer = m_queRecycle.front ();
            m_queRecycle.pop ();
            return pEventBuffer;
        }
    }
    return new SEventBuffer;
}

void CEventBufferRecycle::RecycleEventBuffer(SEventBuffer *pEventBuffer)
{
    std::lock_guard<std::mutex> lk(m_mtxSyncRecycle);

    m_queRecycle.push ( pEventBuffer );
}

}
