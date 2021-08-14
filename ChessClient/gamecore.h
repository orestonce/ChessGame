#ifndef GAMECORE_H
#define GAMECORE_H

#include "CSProtocol.h"
#include <functional>
#include <QPoint>
#include <QVector>

class GamePoint
{
public:
    char Data;

    bool IsEmpty();
    QString GetImageString();
};

class GameCore
{
public:
    static const int LINE_END = 10;
    static const int COLUMN_END = 9;
    static const int HEARTBEAT_OVERTIME_SECONDS = 30;
    static const char EPieceTeamNone = '0';

    GameCore();
    void Load(const SyncPanelMessage& resp);
    GamePoint GetPoint(int iLine, int iColumn);
    GamePoint GetPoint(QPoint p);
    bool AmUpper();
    bool IsPointValied(QPoint p);
    void ClearPoint(QPoint& p);
    bool IsTurnMe();
    bool IsTurnUpper();
    bool SameTeamWithMe(QPoint p);
    bool IsInSuggestionList(PiecePoint p);
public:
    QString WUsername ;
    QString WUserId;
    QString BUsername;
    QString BUserId;
    bool IsGameRunning ;
    QString NextTurnUserId;
    bool ShowSiteDown;
    bool ShowReGame;
    char m_arrChessPanel[LINE_END][ COLUMN_END];
    QPoint SelectedPointFrom;
    QPoint SelectedPointTo;
    QVector<PiecePoint> SuggestionPointToList;
public:
    QString MyUserId;
};

#endif // GAMECORE_H
