#include "EventBuffer.h"
#include <memory.h>

namespace chess
{

void SEventBuffer::Write(UINT uTraceID, UINT uEventType, const void *pEventParamData, UINT uEventParamBytes)
{
    m_uTraceID = uTraceID;
    m_uEventType = uEventType;
    if ( uEventParamBytes )
    {
        m_vecEventParam.resize (uEventParamBytes);
        memcpy (&m_vecEventParam[0], pEventParamData, uEventParamBytes);
    }
}

void *SEventBuffer::GetEventParamPtr()
{
    return &m_vecEventParam[0];
}

}
