#ifndef SEVENTBUFFER_H
#define SEVENTBUFFER_H

#include "RoSpace/RoTypes.h"
#include <vector>

namespace chess
{
using namespace RoSpace;

struct SEventBuffer
{
    UINT m_uTraceID; // 追踪ID
    UINT m_uEventType; // 事件类型
    std::vector<char> m_vecEventParam; // 事件参数
public:
    // 填充一个事件到事件缓冲区
    void Write(UINT uTraceID,UINT uEventType, const void* pEventParamData, UINT uEventParamBytes);
    // 获取事件缓冲
    void *GetEventParamPtr();
};

}

#endif // SEVENTBUFFER_H
