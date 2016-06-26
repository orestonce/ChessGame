#ifndef COMMON_H
#define COMMON_H

namespace chess
{
enum
{
    MAX_USERNAME_LEN = 16, // 最大用户名长度
    MAX_PASSWORD_LEN = 16, // 最大密码长度
    MAX_MSG_LEN = 1024 * 5, // 最大消息长度
};
#define PACKET_HEADER_SIZE 8
#define MAX_PACKET_SIZE (PACKET_HEADER_SIZE + 1000 * 1024)

enum class EMoveErrorCode
{
    CanMove,                                // [0, Piece] 能够移动
    GameIsStoped,                         // [1, Game]游戏已停止
    NotYouTurn,                            // [2, Game]不该你走
    SourceIsNotYours,                     // [3, Piece]起点地址不是你的棋子
    DestnationIsYours,                    // [4, Piece]终点位置是你的棋子
    CanMove_But_YouWillDie,        // [5, Game]这样移动后你就会被一步KILL
    NoMatchPieceRule                     // [6, Piece]移动不符合规则
};

// 棋子类型
enum class EPieceType
{
    Che, // 车
    Ma,  // 马
    Xiang, // 象
    Shi,     // 士
    Jiang,  // 将
    Pao,     // 炮
    Bing,    // 兵
    None    // 没有棋子
};

enum class EPieceTeam // 队伍类型
{
    Up,      // 上方
    Down, // 下方
    None   // 无类型
};
enum class ETakeSiteResult // 坐下操作的结果
{
    TakeSiteOK,      // 坐下成功
    GameIsRuning, // 游戏已经开始了
    SiteHasUser,     // 你坐的位置有人
    YouHaveSited,  // 你丫的正在一个座位上
};

enum class EWinReason
{
    KilledJiang,        // 一方杀死了另一方的将
    Another_NoWay,// 被将死
    Another_Leave,   // 另一方离开了游戏
};

}
#endif // COMMON_H
