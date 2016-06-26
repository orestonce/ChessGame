#ifndef SERVERINTERNAL_H
#define SERVERINTERNAL_H

#include "Common.h"

namespace chess
{

enum
{
    SD_SAVE_MSG = 1000, // 保存消息
};

struct SSD_SAVE_MSG
{
    char m_szMessageFrom[ MAX_USERNAME_LEN +1 ];
    char m_szMessageBody[ MAX_MSG_LEN + 1];
};

}

#endif // SERVERINTERNAL_H
