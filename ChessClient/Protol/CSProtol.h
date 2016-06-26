#ifndef CSPROTOL_H
#define CSPROTOL_H

#include "RoSpace/RoTypes.h"
#include "Common.h"
#include <vector>
#include <string>

namespace chess
{
using namespace RoSpace;

enum
{
    CS_REGISTER,          // 注册用户
    SC_REGISTER_ACK, // 注册用户的回应
    CS_LOGIN,                // 用户登陆
    SC_LOGIN_ACK,       // 用户登陆的回应
    CS_SEND_MSG,        // 客户端发送消息给所有客户端
    SC_PUSH_MSG,        // 服务器推送某个客户端发送的消息

    CS_HEART_BEAT,     // 心跳包
    SC_HEART_BEAT_ACK, // 心跳包回应

    SC_USER_LOGINED,            // 用户登陆消息
    SC_USER_DISCONNECTED, // 用户断开消息

    CS_TAKESITE,                  // 坐下
    SC_TAKESITE_ACK,          // 坐下回应
    CS_MOVE_PIECE,             // 移动棋子
    SC_MOVE_PIECE_ACK,     // 移动棋子回应

    SC_PUSH_CHESS_PANEL, // 推送棋盘
    SC_PUSH_GAME_OVER,   // 推送游戏结束的消息
    SC_PUSH_TURN_CHANGE, // 推送现在行棋的玩家
    SC_PUSH_USER_LEAVE,     // 推送用户离开的消息
    SC_PUSH_JIANGJU,            // 推送将君消息

    CS_REGAME, // 新游戏
};

struct SCS_REGISTER
{
    char m_szUserName[ MAX_USERNAME_LEN + 1];
    char m_szPassword[ MAX_PASSWORD_LEN + 1];
};

struct SSC_REGISTER_ACK
{
    enum
    {
        REGISTER_OK, // 注册成功,
        USERNAME_EXISTS, // 用户名已存在
        UNKOWN_ERROR // 未知错误
    };
    char   m_szUserName[ MAX_USERNAME_LEN +1 ]; // 用户名
    UINT  m_uRegisterResult; //  注册结果
};

// 客户端发到服务器的登陆信息
struct SCS_LOGIN
{
    char m_szUserName[ MAX_USERNAME_LEN+1 ]; // 用户名
    char m_szPassword[ MAX_PASSWORD_LEN+1 ]; // 密码
};

struct SSC_LOGIN_ACK
{
    enum
    {
        LOGIN_OK, // 登陆成功
        PASSWORD_ERROR, //密码错误
        USER_LOGINED, // 用户已登陆
        UNKOWN_ERROR, // 未知错误
    };
    char m_szUserName[ MAX_USERNAME_LEN +1 ]; // 用户名
    UINT m_uLoginResult; // 登陆结果
};

struct SCS_SEND_MSG
{
    char m_szMessageBody[ MAX_MSG_LEN + 1 ];
};

struct SSC_PUSH_MSG
{
    char m_szMessageFrom[ MAX_USERNAME_LEN + 1 ];
    char m_szMessageBody[ MAX_MSG_LEN + 1 ];
};

struct SSC_USER_LOGINED
{
    std::vector< std::string > m_vecLoginUsers;
};

struct SSC_USER_DISCONNECTED
{
    std::vector< std::string > m_vecDisconnectedUsers;
};

struct SSC_TAKESITE_ACK
{
    char m_szUserName[ MAX_USERNAME_LEN + 1];
    UINT m_uDestSite;            // EPieceTeam
    UINT m_uTakeSiteResult;  // ETakeSiteResult ;
};

struct SSC_MOVE_PIECE_ACK
{
    struct SPoint // 一个点
    {
        int m_iLine; // 行
        int m_iRow; // 列
    };
    char m_szUserName[ MAX_USERNAME_LEN +1 ];
    SPoint m_xFrom;
    SPoint m_xTo;
    UINT m_uMoveResult; // EMoveErrorCode
};

struct SCS_MOVE_PIECE
{
    typedef SSC_MOVE_PIECE_ACK::SPoint SPoint;

    SPoint m_xFrom;
    SPoint m_xTo;
};

struct SSC_PUSH_CHESS_PANEL
{
    struct SChessPoint
    {
        typedef SSC_MOVE_PIECE_ACK::SPoint SPoint;

        SPoint m_xPoint;
        USHORT m_uChessType; // EPieceType
        USHORT m_uChessTeam; // EPieceTeam
    };
    enum {
        LINE_END = 10,
        ROW_END = 9
    };
    SChessPoint m_arr[LINE_END][ ROW_END ]; // 整个棋盘的信息
    char m_szUpUserName[ MAX_USERNAME_LEN + 1 ];
    char m_szDownUserName[ MAX_USERNAME_LEN + 1];
    bool m_bIsRuning;
};

struct SSC_PUSH_GAME_OVER
{
    UINT m_uWinnerTeam; // 胜利的一队 EPieceTeam
    UINT m_uWinReason;   // 胜利的原因 EWinReason
};

struct SSC_PUSH_TURN_CHANGE
{
    char m_szTurnNow[MAX_USERNAME_LEN+1]; // 现在行棋的玩家
};

struct SSC_PUSH_USER_LEAVE
{
    char m_szLeaveUserName[ MAX_USERNAME_LEN + 1 ]; // 离开的用户
};

struct SSC_PUSH_JIANGJU
{
    char m_szBeiJiangJuUserName[ MAX_USERNAME_LEN + 1 ]; // 被将军的用户名
};

}
#endif // CSPROTOL_H
