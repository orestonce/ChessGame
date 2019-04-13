// 自动生成的代码, 不要修改!!!
#include "CSProtocol.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

void NoticeBase::RegisterPiecePoint(std::function<void (const PiecePoint&)> fn)
{
    this->funcMap["PiecePoint"] = [=](const QByteArray& bin) {
        PiecePoint message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
PiecePoint::PiecePoint()
{
	this->Line = 0;
	this->Column = 0;
}

QByteArray PiecePoint::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcNotice";
	obj["Method"] = "PiecePoint";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject PiecePoint::ToJsonObject() const
{
	QJsonObject data;
	data["Line"] = this->Line;
	data["Column"] = this->Column;

	return data;
}

bool PiecePoint::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcNotice" || obj["Method"].toString() != "PiecePoint")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool PiecePoint::FromJsonObject(QJsonObject data)
{
	this->Line = data["Line"].toInt();
	this->Column = data["Column"].toInt();

	return true;
}

void NoticeBase::RegisterSyncPanelMessage(std::function<void (const SyncPanelMessage&)> fn)
{
    this->funcMap["SyncPanelMessage"] = [=](const QByteArray& bin) {
        SyncPanelMessage message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
SyncPanelMessage::SyncPanelMessage()
{
	this->IsGameRunning = false;
	this->ShowReGame = false;
	this->ShowSiteDown = false;
}

QByteArray SyncPanelMessage::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcNotice";
	obj["Method"] = "SyncPanelMessage";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject SyncPanelMessage::ToJsonObject() const
{
	QJsonObject data;
	data["PanelFull"] = this->PanelFull;
	data["UpperUsername"] = this->UpperUsername;
	data["DownUsername"] = this->DownUsername;
	data["IsGameRunning"] = this->IsGameRunning;
	data["NextTurnUsername"] = this->NextTurnUsername;
	data["ShowReGame"] = this->ShowReGame;
	data["ShowSiteDown"] = this->ShowSiteDown;

	return data;
}

bool SyncPanelMessage::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcNotice" || obj["Method"].toString() != "SyncPanelMessage")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool SyncPanelMessage::FromJsonObject(QJsonObject data)
{
	this->PanelFull = data["PanelFull"].toString();
	this->UpperUsername = data["UpperUsername"].toString();
	this->DownUsername = data["DownUsername"].toString();
	this->IsGameRunning = data["IsGameRunning"].toBool();
	this->NextTurnUsername = data["NextTurnUsername"].toString();
	this->ShowReGame = data["ShowReGame"].toBool();
	this->ShowSiteDown = data["ShowSiteDown"].toBool();

	return true;
}

void NoticeBase::RegisterChatMessage(std::function<void (const ChatMessage&)> fn)
{
    this->funcMap["ChatMessage"] = [=](const QByteArray& bin) {
        ChatMessage message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
ChatMessage::ChatMessage()
{
}

QByteArray ChatMessage::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcNotice";
	obj["Method"] = "ChatMessage";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ChatMessage::ToJsonObject() const
{
	QJsonObject data;
	data["TimeStr"] = this->TimeStr;
	data["Username"] = this->Username;
	data["Text"] = this->Text;

	return data;
}

bool ChatMessage::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcNotice" || obj["Method"].toString() != "ChatMessage")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ChatMessage::FromJsonObject(QJsonObject data)
{
	this->TimeStr = data["TimeStr"].toString();
	this->Username = data["Username"].toString();
	this->Text = data["Text"].toString();

	return true;
}

void NoticeBase::RegisterGameOverNotice(std::function<void (const GameOverNotice&)> fn)
{
    this->funcMap["GameOverNotice"] = [=](const QByteArray& bin) {
        GameOverNotice message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
GameOverNotice::GameOverNotice()
{
	this->IsPeace = false;
}

QByteArray GameOverNotice::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcNotice";
	obj["Method"] = "GameOverNotice";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject GameOverNotice::ToJsonObject() const
{
	QJsonObject data;
	data["IsPeace"] = this->IsPeace;
	data["WinUsername"] = this->WinUsername;

	return data;
}

bool GameOverNotice::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcNotice" || obj["Method"].toString() != "GameOverNotice")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool GameOverNotice::FromJsonObject(QJsonObject data)
{
	this->IsPeace = data["IsPeace"].toBool();
	this->WinUsername = data["WinUsername"].toString();

	return true;
}

void NoticeBase::RegisterServerKickYou(std::function<void (const ServerKickYou&)> fn)
{
    this->funcMap["ServerKickYou"] = [=](const QByteArray& bin) {
        ServerKickYou message;
        if (!message.DecodeFromQByteArray(bin)) {
            return;
        }
        fn(message);
    };
}
ServerKickYou::ServerKickYou()
{
}

QByteArray ServerKickYou::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcNotice";
	obj["Method"] = "ServerKickYou";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ServerKickYou::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool ServerKickYou::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcNotice" || obj["Method"].toString() != "ServerKickYou")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ServerKickYou::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
LoginRequest::LoginRequest()
{
}

QByteArray LoginRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcLogin";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject LoginRequest::ToJsonObject() const
{
	QJsonObject data;
	data["Username"] = this->Username;
	data["Password"] = this->Password;
	data["RoomId"] = this->RoomId;

	return data;
}

bool LoginRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcLogin")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool LoginRequest::FromJsonObject(QJsonObject data)
{
	this->Username = data["Username"].toString();
	this->Password = data["Password"].toString();
	this->RoomId = data["RoomId"].toString();

	return true;
}
LoginResponse::LoginResponse()
{
}

QByteArray LoginResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcLogin";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject LoginResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool LoginResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcLogin")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool LoginResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
PingRequest::PingRequest()
{
}

QByteArray PingRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcPing";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject PingRequest::ToJsonObject() const
{
	QJsonObject data;

	return data;
}

bool PingRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcPing")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool PingRequest::FromJsonObject(QJsonObject )
{

	return true;
}
PingResponse::PingResponse()
{
}

QByteArray PingResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcPing";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject PingResponse::ToJsonObject() const
{
	QJsonObject data;

	return data;
}

bool PingResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcPing")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool PingResponse::FromJsonObject(QJsonObject )
{

	return true;
}
RegisterRequest::RegisterRequest()
{
}

QByteArray RegisterRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcRegister";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject RegisterRequest::ToJsonObject() const
{
	QJsonObject data;
	data["Username"] = this->Username;
	data["Password"] = this->Password;

	return data;
}

bool RegisterRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcRegister")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool RegisterRequest::FromJsonObject(QJsonObject data)
{
	this->Username = data["Username"].toString();
	this->Password = data["Password"].toString();

	return true;
}
RegisterResponse::RegisterResponse()
{
}

QByteArray RegisterResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcRegister";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject RegisterResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool RegisterResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcRegister")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool RegisterResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
ChatRequest::ChatRequest()
{
}

QByteArray ChatRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcChat";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ChatRequest::ToJsonObject() const
{
	QJsonObject data;
	data["Text"] = this->Text;

	return data;
}

bool ChatRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcChat")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ChatRequest::FromJsonObject(QJsonObject data)
{
	this->Text = data["Text"].toString();

	return true;
}
ChatResponse::ChatResponse()
{
}

QByteArray ChatResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcChat";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ChatResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool ChatResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcChat")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ChatResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
GetSuggestionRequest::GetSuggestionRequest()
{
}

QByteArray GetSuggestionRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcGetSuggestion";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject GetSuggestionRequest::ToJsonObject() const
{
	QJsonObject data;
	data["FromPoint"] = this->FromPoint.ToJsonObject();

	return data;
}

bool GetSuggestionRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcGetSuggestion")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool GetSuggestionRequest::FromJsonObject(QJsonObject data)
{
	this->FromPoint.FromJsonObject(data["FromPoint"].toObject());

	return true;
}
GetSuggestionResponse::GetSuggestionResponse()
{
}

QByteArray GetSuggestionResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcGetSuggestion";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject GetSuggestionResponse::ToJsonObject() const
{
	QJsonObject data;
	data["FromPoint"] = this->FromPoint.ToJsonObject();

	{
		QJsonArray array;
		for (auto it=this->CanMoveToList.begin(); it != this->CanMoveToList.end(); it++)
		{
			array.append( it->ToJsonObject() );
		}
		data["CanMoveToList"] = array;
	}
	return data;
}

bool GetSuggestionResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcGetSuggestion")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool GetSuggestionResponse::FromJsonObject(QJsonObject data)
{
	this->FromPoint.FromJsonObject(data["FromPoint"].toObject());

	{
		QJsonArray array = data["CanMoveToList"].toArray();
		for (auto it=array.begin(); it != array.end(); it++)
		{
			PiecePoint temp;
			temp.FromJsonObject((*it).toObject());
			this->CanMoveToList.push_back(temp);
		}
	}
	return true;
}
MovePieceRequest::MovePieceRequest()
{
}

QByteArray MovePieceRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcMovePiece";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject MovePieceRequest::ToJsonObject() const
{
	QJsonObject data;
	data["FromPoint"] = this->FromPoint.ToJsonObject();
	data["ToPoint"] = this->ToPoint.ToJsonObject();

	return data;
}

bool MovePieceRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcMovePiece")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool MovePieceRequest::FromJsonObject(QJsonObject data)
{
	this->FromPoint.FromJsonObject(data["FromPoint"].toObject());
	this->ToPoint.FromJsonObject(data["ToPoint"].toObject());

	return true;
}
MovePieceResponse::MovePieceResponse()
{
}

QByteArray MovePieceResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcMovePiece";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject MovePieceResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool MovePieceResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcMovePiece")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool MovePieceResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
ReGameRequest::ReGameRequest()
{
}

QByteArray ReGameRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcReGame";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ReGameRequest::ToJsonObject() const
{
	QJsonObject data;

	return data;
}

bool ReGameRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcReGame")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ReGameRequest::FromJsonObject(QJsonObject )
{

	return true;
}
ReGameResponse::ReGameResponse()
{
}

QByteArray ReGameResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcReGame";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject ReGameResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool ReGameResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcReGame")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool ReGameResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}
TakeSiteRequest::TakeSiteRequest()
{
}

QByteArray TakeSiteRequest::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcCall";
	obj["Method"] = "RpcTakeSite";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject TakeSiteRequest::ToJsonObject() const
{
	QJsonObject data;

	return data;
}

bool TakeSiteRequest::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcCall" || obj["Method"].toString() != "RpcTakeSite")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool TakeSiteRequest::FromJsonObject(QJsonObject )
{

	return true;
}
TakeSiteResponse::TakeSiteResponse()
{
}

QByteArray TakeSiteResponse::EncodeToQByteArray() const
{
	QJsonObject obj;
    obj["PacketType"] = "PtRpcReply";
	obj["Method"] = "RpcTakeSite";
	obj["Data"] = this->ToJsonObject();

	QJsonDocument doc;
    doc.setObject(obj);
    return doc.toJson();
}

QJsonObject TakeSiteResponse::ToJsonObject() const
{
	QJsonObject data;
	data["ErrMsg"] = this->ErrMsg;

	return data;
}

bool TakeSiteResponse::DecodeFromQByteArray(const QByteArray& bin)
{
	QJsonObject obj = QJsonDocument::fromJson(bin).object();
	if (obj["PacketType"].toString() != "PtRpcReply" || obj["Method"].toString() != "RpcTakeSite")
	{
		return false;
	}
	return this->FromJsonObject(obj["Data"].toObject());
}

bool TakeSiteResponse::FromJsonObject(QJsonObject data)
{
	this->ErrMsg = data["ErrMsg"].toString();

	return true;
}

void NoticeBase::OnNoticeMessage(const QString& method, const QByteArray& bin)
{
    auto it = this->funcMap.find(method);
    if (it == this->funcMap.end())
    {
        return;
    }
    auto func = it->second;	// 复制出来调用才安全!!!
	func(bin);
}
void NoticeBase::ClearNoticeCallback()
{
	this->funcMap.clear();
}
