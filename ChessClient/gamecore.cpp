#include "gamecore.h"
#include <QDebug>

void GameCore::Load(const SyncPanelMessage &resp)
{
    QStringList lineData = resp.PanelFull.split(QChar('/'));
    for (int line=0; line < LINE_END; ++line) {
        if (line >= lineData.length()) {
            break;
        }
        int rIdx =0;
        for(int col =0; col < COLUMN_END; ) {
            if (rIdx >= lineData[line].length()) {
                break;
            }
            char ch = lineData[line][rIdx].toLatin1();
            rIdx++;
            if ('0' < ch && ch <= '9') {
                int emptyCnt = int(int(ch) - '0');
                while (emptyCnt > 0 && col < COLUMN_END) {
                    m_arrChessPanel[line][col] = EPieceTeamNone;
                    col++;
                    emptyCnt--;
                }
            } else {
                m_arrChessPanel[line][col] = ch;
                col++;
            }
        }
    }
    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iColumn=0; iColumn<COLUMN_END; ++iColumn)
        {
            int idx = iLine*COLUMN_END + iColumn;
            if (idx > LINE_END * COLUMN_END ) {
                continue;
            }
        }
    }
    this->WUsername = resp.WUserName;
    this->WUserId = resp.WUserId;
    this->BUsername = resp.BUserName;
    this->BUserId = resp.BUserId;
    this->IsGameRunning = resp.IsGameRunning;
    this->NextTurnUserId = resp.NextTurnUserId;
    this->ShowReGame = resp.ShowReGame;
    this->ShowSiteDown = resp.ShowSiteDown;
}

GamePoint GameCore::GetPoint(int iLine, int iColumn)
{
    GamePoint p;
    p.Data = this->m_arrChessPanel[iLine][iColumn];
    return p;
}

GamePoint GameCore::GetPoint(QPoint p)
{
    return this->GetPoint(p.y(), p.x());
}

bool GameCore::IsBlack()
{
    return this->BUserId == this->MyUserId;
}

bool GameCore::IsPointValied(QPoint p)
{
    int x=p.x();
    int y=p.y();
    return 0 <= x && x < COLUMN_END && 0 <= y && y < LINE_END;
}

void GameCore::ClearPoint(QPoint &p)
{
    p = QPoint(-1, -1);
}

bool GameCore::IsTurnMe()
{
    return this->NextTurnUserId == this->MyUserId;
}

bool GameCore::IsTurnW()
{
    return this->NextTurnUserId == this->WUserId;
}

bool GameCore::SameTeamWithMe(QPoint p)
{
    auto data = this->GetPoint(p).Data;
    if (this->MyUserId == this->BUserId) {
        return islower(data);
    }
    if (this->MyUserId == this->WUserId ) {
        return isupper(data);
    }
    return false;
}

bool GameCore::IsInSuggestionList(PiecePoint p)
{
    for (auto it=SuggestionPointToList.begin(); it != SuggestionPointToList.end(); ++it)
    {
        if (p.Y == it->Y && p.X == it->X)
        {
            return true;
        }
    }
    return false;
}

QString GameCore::FormatPanel()
{
    QString out;
    for (int iLine =0; iLine < LINE_END; iLine++) {
        int emptyCount = 0;
        for (int iCol =0; iCol < COLUMN_END; iCol++) {
            if (this->m_arrChessPanel[iLine][iCol] == '0' ) {
                emptyCount++;
                continue;
            }
            if (emptyCount>0) {
                out.append(QChar('0' + emptyCount));
                emptyCount=0;
            }
            out.append(QChar(this->m_arrChessPanel[iLine][iCol]));
        }
        if (emptyCount > 0) {
            out.append(QChar('0' + emptyCount));
        }
        if (iLine == LINE_END-1) {
            out.append(QChar(' '));
        } else {
            out.append(QChar('/'));
        }
    }
    if (this->NextTurnUserId == this->WUserId) {
        out.append(QChar('w'));
    } else {
        out.append(QChar('b'));
    }
    return out;
}

GameCore::GameCore()
{
    this->IsGameRunning = false;
    this->ShowReGame = false;
    this->ShowSiteDown = false;
    //对用来接收棋子初始化信息的二维数组进行空值处理
    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iColumn=0; iColumn<COLUMN_END; ++iColumn)
        {
            m_arrChessPanel[iLine][iColumn]= EPieceTeamNone;
        }
    }
    this->ClearPoint(this->SelectedPointFrom);
    this->ClearPoint(this->SelectedPointTo);
}


bool GamePoint::IsEmpty()
{
    return this->Data == GameCore::EPieceTeamNone;
}

QString GamePoint::GetImageString()
{
    QString qstrImagePath ;
    if (!islower(this->Data))
    {
        qstrImagePath = "w";
    }
    else
    {
        qstrImagePath = "b";
    }
    qstrImagePath.push_back(tolower(this->Data));
    return  ":/images/" + qstrImagePath + ".png";
}
