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
    static const int MAX_VALUE_Y = 10;
    static const int MAX_VALUE_X = 9;
    static const int HEARTBEAT_OVERTIME_SECONDS = 30;
    static const char EPieceTeamNone = '0';

    GameCore();
    void Load(const SyncPanelMessage& resp);
    GamePoint GetPoint(int iLine, int iColumn);
    GamePoint GetPoint(QPoint p);
    bool IsBlack();
    bool IsPointValied(QPoint p);
    void ClearPoint(QPoint& p);
    bool IsTurnMe();
    bool IsTurnW();
    bool SameTeamWithMe(QPoint p);
    bool IsInSuggestionList(PiecePoint p);
    QString FormatPanel();
public:
    QString WUsername ;
    QString WUserId;
    QString BUsername;
    QString BUserId;
    bool IsGameRunning ;
    QString NextTurnUserId;
    bool ShowSiteDown;
    bool ShowReGame;
    char m_arrChessPanel[MAX_VALUE_Y][ MAX_VALUE_X];
    QPoint SelectedPointFrom;
    QPoint SelectedPointTo;
    QVector<PiecePoint> SuggestionPointToList;
public:
    QString MyUserId;
};

#endif // GAMECORE_H
