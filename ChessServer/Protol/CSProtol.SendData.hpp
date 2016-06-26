#ifndef CSPROTOL_SENDDATA_HPP
#define CSPROTOL_SENDDATA_HPP

#include <string>
#include "CSProtol.Serialize.hpp"
#include "RoNet/RoNet.h"
#include "RoLog/RoLog.h"

namespace chess
{
template <typename SCStruct>
bool SendData(UINT uConnIndex, const SCStruct& xStruct, UINT uEventType)
{
    std::string strPakcet;

    if ( !SerializeToString(xStruct, uEventType, strPakcet) )
    {
        START_LOG(RO_CERR).OUT_LOG("Packet too big !").END_LOG();
        return false;
    }
    return RoNet::SendData (uConnIndex, &strPakcet[0], strPakcet.size ());
}

inline bool SendData(UINT uConnIndex, UINT uEventType)
{
    char xBuffer[ PACKET_HEADER_SIZE ];
    char* ptr = xBuffer;

    *( UINT* ) ptr = htonl ( PACKET_HEADER_SIZE );
    ptr += 4;

    *(UINT* ) ptr = htonl ( uEventType );

    return RoNet::SendData (uConnIndex, xBuffer, PACKET_HEADER_SIZE);
}

}

#endif // CSPROTOL_SENDDATA_HPP
