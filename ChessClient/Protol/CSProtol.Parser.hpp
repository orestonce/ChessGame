#ifndef CSPROTOL_PARSER_HPP
#define CSPROTOL_PARSER_HPP

#include "CSProtol.h"
#include "RoSpace/RoTypes.h"
#include "CSProtol.Serialize.hpp"

namespace chess
{
using namespace RoSpace;

/**
 * @brief The SPacket struct 解析出来的数据包
 */
struct SPacket
{
    UINT m_uEventType;            // 事件类型
    const char* pParamBegin;    // 事件参数起始迭代器
    const char* pParamEnd;       // 事件参数结束迭代器
};

/**
 * @brief ParsePacket
 * @param sPacket
 * @param pPacket
 * @param uPacketBytes
 * @return
 */
bool ParsePacket(SPacket& sPacket, const char* pPacket, UINT uPacketBytes);

}
#endif // CSPROTOL_PARSER_HPP
