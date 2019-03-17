// 自动生成的代码, 不要修改!!!
#pragma once
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QJsonObject>

#include <functional>
#include <map>

struct RpcAbstract
{
public :
    virtual bool DecodeFromQByteArray(const QByteArray& bin)=0;
    virtual QByteArray EncodeToQByteArray() const=0;
	virtual QString GetMethod() const=0;
};

typedef struct PiecePoint : public RpcAbstract
{
public :
	PiecePoint();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("PiecePoint"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	int Line ;
	int Column ;
} PiecePoint;

typedef struct SyncPanelMessage : public RpcAbstract
{
public :
	SyncPanelMessage();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("SyncPanelMessage"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString PanelFull ;
	QString UpperUsername ;
	QString DownUsername ;
	bool IsGameRunning ;
	QString NextTurnUsername ;
	bool ShowReGame ;
	bool ShowSiteDown ;
} SyncPanelMessage;

typedef struct ChatMessage : public RpcAbstract
{
public :
	ChatMessage();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("ChatMessage"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString TimeStr ;
	QString Username ;
	QString Text ;
} ChatMessage;

typedef struct GameOverNotice : public RpcAbstract
{
public :
	GameOverNotice();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("GameOverNotice"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	bool IsPeace ;
	QString WinUsername ;
} GameOverNotice;

typedef struct ServerKickYou : public RpcAbstract
{
public :
	ServerKickYou();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("ServerKickYou"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} ServerKickYou;

typedef struct LoginRequest : public RpcAbstract
{
public :
	LoginRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcLogin"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString Username ;
	QString Password ;
	QString RoomId ;
} LoginRequest;

typedef struct LoginResponse : public RpcAbstract
{
public :
	LoginResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcLogin"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} LoginResponse;

typedef struct PingRequest : public RpcAbstract
{
public :
	PingRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcPing"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
} PingRequest;

typedef struct PingResponse : public RpcAbstract
{
public :
	PingResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcPing"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
} PingResponse;

typedef struct RegisterRequest : public RpcAbstract
{
public :
	RegisterRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcRegister"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString Username ;
	QString Password ;
} RegisterRequest;

typedef struct RegisterResponse : public RpcAbstract
{
public :
	RegisterResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcRegister"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} RegisterResponse;

typedef struct ChatRequest : public RpcAbstract
{
public :
	ChatRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcChat"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString Text ;
} ChatRequest;

typedef struct ChatResponse : public RpcAbstract
{
public :
	ChatResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcChat"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} ChatResponse;

typedef struct GetSuggestionRequest : public RpcAbstract
{
public :
	GetSuggestionRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcGetSuggestion"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	PiecePoint FromPoint ;
} GetSuggestionRequest;

typedef struct GetSuggestionResponse : public RpcAbstract
{
public :
	GetSuggestionResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcGetSuggestion"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	PiecePoint FromPoint ;
	QVector<PiecePoint> CanMoveToList ;
} GetSuggestionResponse;

typedef struct MovePieceRequest : public RpcAbstract
{
public :
	MovePieceRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcMovePiece"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	PiecePoint FromPoint ;
	PiecePoint ToPoint ;
} MovePieceRequest;

typedef struct MovePieceResponse : public RpcAbstract
{
public :
	MovePieceResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcMovePiece"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} MovePieceResponse;

typedef struct ReGameRequest : public RpcAbstract
{
public :
	ReGameRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcReGame"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
} ReGameRequest;

typedef struct ReGameResponse : public RpcAbstract
{
public :
	ReGameResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcReGame"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} ReGameResponse;

typedef struct TakeSiteRequest : public RpcAbstract
{
public :
	TakeSiteRequest();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcTakeSite"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
} TakeSiteRequest;

typedef struct TakeSiteResponse : public RpcAbstract
{
public :
	TakeSiteResponse();
	virtual bool DecodeFromQByteArray(const QByteArray& bin) override final;
	virtual QByteArray EncodeToQByteArray() const override final;
	virtual QString GetMethod() const override final { return QString("RpcTakeSite"); }
	QJsonObject ToJsonObject() const;
	bool FromJsonObject(QJsonObject obj);
public :
	QString ErrMsg ;
} TakeSiteResponse;

class NoticeBase
{
public :
	void RegisterPiecePoint(std::function<void (const PiecePoint& message)> fn);
	void RegisterSyncPanelMessage(std::function<void (const SyncPanelMessage& message)> fn);
	void RegisterChatMessage(std::function<void (const ChatMessage& message)> fn);
	void RegisterGameOverNotice(std::function<void (const GameOverNotice& message)> fn);
	void RegisterServerKickYou(std::function<void (const ServerKickYou& message)> fn);
public:
    void OnNoticeMessage(const QString& method, const QByteArray& bin);
	void ClearNoticeCallback();
private:
    std::map<QString, std::function<void (const QByteArray& bin)> > funcMap;
};
