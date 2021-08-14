#include "gamecore.h"

void GameCore::Load(const SyncPanelMessage &resp)
{
    std::string full = resp.PanelFull.toStdString();

    for(int iLine=0; iLine<LINE_END; ++iLine)
    {
        for(int iColumn=0; iColumn<COLUMN_END; ++iColumn)
        {
            int idx = iLine*COLUMN_END + iColumn;
            if (idx > LINE_END * COLUMN_END ) {
                continue;
            }
            m_arrChessPanel[iLine][iColumn]= full[idx];
        }
    }
    this->WUsername = resp.UpperUsername;
    this->BUsername = resp.DownUsername;
    this->IsGameRunning = resp.IsGameRunning;
    this->NextTurnUsername = resp.NextTurnUsername;
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

bool GameCore::AmUpper()
{
    return this->WUsername == this->MyUsername;
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
    return this->NextTurnUsername == this->MyUsername;
}

bool GameCore::IsTurnUpper()
{
    return this->NextTurnUsername == this->WUsername;
}

bool GameCore::SameTeamWithMe(QPoint p)
{
    auto data = this->GetPoint(p).Data;
    if (this->MyUsername == this->WUsername) {
        return islower(data);
    }
    if (this->MyUsername == this->BUsername ) {
        return isupper(data);
    }
    return false;
}

bool GameCore::IsInSuggestionList(PiecePoint p)
{
    for (auto it=SuggestionPointToList.begin(); it != SuggestionPointToList.end(); ++it)
    {
        if (p.Line == it->Line && p.Column == it->Column)
        {
            return true;
        }
    }
    return false;
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
