#include "CSProtol.h"
#include "CSProtol.Parser.hpp"

namespace chess
{

bool ParsePacket (SPacket &sPacket, const char *pPacket, UINT uPacketBytes)
{
    if ( uPacketBytes < PACKET_HEADER_SIZE )
    {
        return false;
    }
    const char* ptr = pPacket + 4;

    sPacket.m_uEventType = ntohl ( *(const UINT*) ptr );
    ptr += 4;

    if ( uPacketBytes == MAX_PACKET_SIZE )
    {
        sPacket.pParamBegin = sPacket.pParamEnd = NULL;
    }
    else
    {
        sPacket.pParamBegin = ptr;
        sPacket.pParamEnd = pPacket + uPacketBytes;
    }
    return true;
}

}
