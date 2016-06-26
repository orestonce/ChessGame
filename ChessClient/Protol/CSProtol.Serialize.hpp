#ifndef CSPROTOL_SERIALIZE_HPP
#define CSPROTOL_SERIALIZE_HPP

#include "CSProtol.h"
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

namespace chess
{

template <typename Archive>
void serialize(Archive& ar, SCS_REGISTER& reg)
{
    ar(reg.m_szUserName, reg.m_szPassword);
}

template <typename Archive>
void serialize(Archive& ar, SSC_REGISTER_ACK& regAck)
{
    ar(regAck.m_szUserName, regAck.m_uRegisterResult);
}

template <typename Archive>
void serialize(Archive& ar, SCS_LOGIN& login)
{
    ar(login.m_szUserName, login.m_szPassword);
}

template <typename Archive>
void serialize(Archive& ar, SSC_LOGIN_ACK& loginAck)
{
    ar(loginAck.m_szUserName, loginAck.m_uLoginResult);
}

template <typename Archive>
void serialize(Archive& ar, SSC_PUSH_MSG& pushMsg)
{
    ar(pushMsg.m_szMessageFrom, pushMsg.m_szMessageBody);
}

template <typename Archive>
void serialize(Archive &ar, SCS_SEND_MSG& sendMsg)
{
    ar(sendMsg.m_szMessageBody);
}

template <typename Archive>
void serialize(Archive &ar, SSC_USER_LOGINED& userLogined)
{
    ar(userLogined.m_vecLoginUsers);
}

template <typename Archive>
void serialize(Archive &ar, SSC_USER_DISCONNECTED& userDisconn)
{
    ar(userDisconn.m_vecDisconnectedUsers);
}

template <typename Archive>
void serialize(Archive &ar, SSC_TAKESITE_ACK& takeSiteAck)
{
    ar(takeSiteAck.m_szUserName, takeSiteAck.m_uDestSite, takeSiteAck.m_uTakeSiteResult);
}

template <typename Archive>
void serialize(Archive &ar, SSC_MOVE_PIECE_ACK& xMovePieceAck)
{
    ar(xMovePieceAck.m_xFrom.m_iLine);
    ar(xMovePieceAck.m_xFrom.m_iRow);
    ar(xMovePieceAck.m_xTo.m_iLine);
    ar(xMovePieceAck.m_xTo.m_iRow);
    ar(xMovePieceAck.m_szUserName);
    ar(xMovePieceAck.m_uMoveResult);
}

template <typename Archive>
void serialize(Archive &ar, SCS_MOVE_PIECE& xMovePiece)
{
    ar(xMovePiece.m_xFrom.m_iLine);
    ar(xMovePiece.m_xFrom.m_iRow);
    ar(xMovePiece.m_xTo.m_iLine);
    ar(xMovePiece.m_xTo.m_iRow);
}

template <typename Archive>
void serialize(Archive &ar, SSC_PUSH_CHESS_PANEL& xChessPanel)
{
    for(int iLine=0; iLine<SSC_PUSH_CHESS_PANEL::LINE_END; ++iLine)
    {
        for(int iRow=0; iRow<SSC_PUSH_CHESS_PANEL::ROW_END; ++iRow)
        {
            auto & xp = xChessPanel.m_arr[iLine][iRow];
            ar(xp.m_uChessTeam);
            ar(xp.m_uChessType);
            ar(xp.m_xPoint.m_iLine);
            ar(xp.m_xPoint.m_iRow);
        }
    }

    ar(xChessPanel.m_bIsRuning);
    ar(xChessPanel.m_szDownUserName);
    ar(xChessPanel.m_szUpUserName);
}

template <typename Archive>
void serialize(Archive &ar, SSC_PUSH_GAME_OVER &xGameOver)
{
    ar(xGameOver.m_uWinnerTeam);
    ar(xGameOver.m_uWinReason);
}

template <typename Archive>
void serialize(Archive &ar, SSC_PUSH_TURN_CHANGE& xTurnChange)
{
    ar(xTurnChange.m_szTurnNow);
}

template <typename Archive>
void serialize(Archive &ar,SSC_PUSH_USER_LEAVE& xLeave)
{
    ar(xLeave.m_szLeaveUserName);
}

template <typename Archive>
void serialize(Archive &ar,SSC_PUSH_JIANGJU& xJiangJu)
{
    ar(xJiangJu.m_szBeiJiangJuUserName);
}

template <typename CSStruct>
bool GetStruct(const char* pParamBegin, const char* pParamEnd, CSStruct& xStruct)
{
    try
    {
        std::istringstream iss( std::string(pParamBegin, pParamEnd) );
        cereal::BinaryInputArchive bin_iar(iss);
        bin_iar >> xStruct;
    }
    catch (const std::exception& e )
    {
        return false;
    }
    return true;
}

template <typename SCStruct>
void PutStruct(const SCStruct& xStruct, std::string& strOutput)
{
    std::ostringstream oss;
    {
        cereal::BinaryOutputArchive bin_oar(oss);
        bin_oar << xStruct;
    }
    strOutput = oss.str ();
}

template<typename PacketType>
bool SerializeToString(const PacketType& xStruct, UINT uEventType, std::string& strOutput)
{
    std::string strTempStore;

    PutStruct(xStruct, strTempStore);

    UINT uPacketBytes = strTempStore.size () + PACKET_HEADER_SIZE;

    if ( uPacketBytes > MAX_PACKET_SIZE )
    {
        return false;
    }
    else
    {
        char xHeaderBuffer[ PACKET_HEADER_SIZE ];
        char* ptr = xHeaderBuffer;

        *(UINT*) ptr = htonl( uPacketBytes );
        ptr += 4;

        *(UINT*) ptr = htonl( uEventType );

        strOutput.clear ();
        strOutput.reserve (uPacketBytes);

        strOutput.append (xHeaderBuffer, xHeaderBuffer+PACKET_HEADER_SIZE);
        strOutput += std::move(strTempStore);
    }
    return true;
}

inline bool SerializeToString(UINT uEventType, std::string& strOutput)
{
    char xBuffer[ PACKET_HEADER_SIZE ];

    char* ptr = xBuffer;

    *(UINT*) ptr = htonl (PACKET_HEADER_SIZE);
    ptr += 4;

    *(UINT*) ptr = htonl (uEventType);

    strOutput.assign (xBuffer, xBuffer + PACKET_HEADER_SIZE);

    return true;
}

}

#endif // CSPROTOL_SERIALIZE_HPP
