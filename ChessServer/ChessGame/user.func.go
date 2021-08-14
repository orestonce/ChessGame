package ChessGame

import (
	"context"
	"github.com/google/uuid"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/dsession"
	"github.com/orestonce/ChessGame/ent/duser"
	"github.com/orestonce/ChessGame/ymd/ymdTime"
	"golang.org/x/crypto/bcrypt"
	"log"
	"time"
)

const (
	ErrUsernameExists          = `用户名已注册`
	ErrUsernameNotExists       = `用户不存在`
	ErrPassword                = `密码错误`
	ErrRoomIdInvalid           = `房间号码不合法`
	ErrGameIsRunning           = `游戏正在运行`
	ErrUserNotEnough           = `玩家不足`
	ErrGameIsNotRunning        = `游戏未开始`
	ErrCannotMove              = `不能这样走`
	ErrNoPosition              = `没有位置了`
	ErrLoginRepeat             = `重复登陆`
	ErrNextLogin               = `你的账号在别处登录`
	ErrEmptyPasswordOrUsername = `账号或密码为空`
	ErrUnknown                 = `未知错误`
)

type PingRequest struct{}
type PingResponse struct{}

func (this *GameUser) RpcPing(req PingRequest) (resp PingResponse) {
	return
}

type RegisterRequest struct {
	Username string
	Password string
}

type RegisterResponse struct {
	ErrMsg string
}

func (this *GameUser) RpcRegister(req RegisterRequest) (resp RegisterResponse) {
	if req.Username == `` || req.Password == `` {
		resp.ErrMsg = ErrEmptyPasswordOrUsername
		return
	}
	exists, err := gDbClient.DUser.Query().Where(duser.Name(req.Username)).Exist(context.Background())
	if err != nil {
		resp.ErrMsg = ErrUnknown
		log.Println("GameUser.RpcRegister", err)
		return resp
	}
	if exists {
		resp.ErrMsg = ErrUsernameExists
		return resp
	}
	hashByte, err := bcrypt.GenerateFromPassword([]byte(req.Password), bcrypt.DefaultCost)
	if err != nil {
		log.Println("GameUser.RpcRegister", err)
		resp.ErrMsg = ErrUnknown
		return resp
	}
	_, err = gDbClient.DUser.Create().SetID(uuid.NewString()).SetName(req.Username).SetPasswordHash(string(hashByte)).Save(context.Background())
	if err != nil {
		log.Println("GameUser.RpcRegister", err)
		resp.ErrMsg = ErrUnknown
		return resp
	}
	return
}

type LoginRequest struct {
	Username string
	Password string
	RoomId   string
}

type LoginResponse struct {
	ErrMsg string
}

func (this *GameUser) RpcLogin(req LoginRequest) (resp LoginResponse) {
	user, err := gDbClient.DUser.Query().Where(duser.Name(req.Username)).Only(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); ok {
			resp.ErrMsg = ErrUsernameNotExists
			return resp
		}
		log.Println("GameUser.RpcLogin", err)
		resp.ErrMsg = ErrPassword
		return resp
	}
	err = bcrypt.CompareHashAndPassword([]byte(user.PasswordHash), []byte(req.Password))
	if err != nil {
		resp.ErrMsg = ErrPassword
		return resp
	}
	if req.RoomId == `` {
		resp.ErrMsg = ErrRoomIdInvalid
		return
	}
	if this.Session.UserName != `` || this.Session.RoomID != `` {
		resp.ErrMsg = ErrLoginRepeat
		return
	}
	_, err = gDbClient.DSession.Update().Where(dsession.ID(this.Session.ID)).SetUserID(user.ID).SetUserName(user.Name).SetRoomID(req.RoomId).Save(context.Background())
	if err != nil {
		log.Println("DSession.Update", err)
		resp.ErrMsg = ErrUnknown
		return
	}
	this.Session.UserID = user.ID
	this.Session.UserName = user.Name
	this.Session.RoomID = req.RoomId
	gGame.OnUserLogin(this)
	return
}

type ChatRequest struct {
	Text string
}

type ChatResponse struct {
	ErrMsg string
}

type ChatMessage struct {
	TimeStr  string
	Username string
	Text     string
}

func (this *GameRoom) RpcChat(session *ent.DSession, req ChatRequest) (resp ChatResponse) {
	message := ChatMessage{
		TimeStr:  ymdTime.DefaultFormat(time.Now()),
		Username: session.UserName,
		Text:     req.Text,
	}
	for _, session := range this.getSessionList() {
		sendNoticeToSession(session.ID, message)
	}
	return
}

func (this *GameRoom) getSessionList() []*ent.DSession {
	return getSessionListBy(dsession.RoomID(this.RoomId))
}

type SyncPanelMessage struct {
	PanelFull        string
	UpperUsername    string
	DownUsername     string
	IsGameRunning    bool
	NextTurnUsername string
	ShowReGame       bool
	ShowSiteDown     bool
}

type TakeSiteRequest struct{}

type TakeSiteResponse struct {
	ErrMsg string
}

func (this *GameRoom) RpcTakeSite(session *ent.DSession, req TakeSiteRequest) (resp TakeSiteResponse) {
	if this.Data.IsGameRunning {
		resp.ErrMsg = ErrGameIsRunning
		return
	}
	userId := session.UserID

	if this.Data.UpUserID == `` && this.Data.DownUserID != userId {
		this.Data.UpUserID = userId
	} else if this.Data.DownUserID == `` && this.Data.UpUserID != userId {
		this.Data.DownUserID = userId
	} else {
		resp.ErrMsg = ErrNoPosition
		return
	}
	if this.Data.DownUserID != `` && this.Data.UpUserID != `` {
		this.Data.IsGameRunning = true
		this.NextTurnUserId = this.Data.DownUserID
		this.LoadPanelFromData()
	}
	this.sync2Client(nil)

	return
}

type PiecePoint struct {
	Line   int32
	Column int32
}

type MovePieceRequest struct {
	FromPoint PiecePoint
	ToPoint   PiecePoint
}

type MovePieceResponse struct {
	ErrMsg string
}

func (this *GameRoom) RpcMovePiece(session *ent.DSession, req MovePieceRequest) (resp MovePieceResponse) {
	if !this.Data.IsGameRunning {
		resp.ErrMsg = ErrGameIsNotRunning
		return
	}
	if !this.CanMove(session, req.FromPoint, req.ToPoint) {
		resp.ErrMsg = ErrCannotMove
		return
	}
	this.DoMove(req.FromPoint, req.ToPoint)
	return
}

type GetSuggestionRequest struct {
	FromPoint PiecePoint
}

type GetSuggestionResponse struct {
	FromPoint     PiecePoint
	CanMoveToList []PiecePoint
}

func (this *GameRoom) RpcGetSuggestion(session *ent.DSession, req GetSuggestionRequest) (resp GetSuggestionResponse) {
	for line := int32(0); line < LINE_COUNT; line++ {
		for column := int32(0); column < COLUMN_COUNT; column++ {
			to := PiecePoint{Line: line, Column: column}
			if this.CanMove(session, req.FromPoint, to) {
				resp.CanMoveToList = append(resp.CanMoveToList, to)
			}
		}
	}
	resp.FromPoint = req.FromPoint

	return
}

type ReGameRequest struct{}
type ReGameResponse struct {
	ErrMsg string
}

func (this *GameRoom) RpcReGame(session *ent.DSession, req ReGameRequest) (resp ReGameResponse) {
	if this.Data.IsGameRunning {
		resp.ErrMsg = ErrGameIsRunning
		return
	}
	if this.Data.DownUserID == `` || this.Data.UpUserID == `` {
		resp.ErrMsg = ErrUserNotEnough
		return
	}
	this.Data.IsGameRunning = true
	this.LoadPanelFromData()
	this.NextTurnUserId = this.Data.UpUserID
	this.sync2Client(nil)
	return
}

func (this *GameRoom) sync2Client(user *GameUser) {
	var s SyncPanelMessage
	this.formatPanelFull(&s)
	var sessionList []*ent.DSession
	if user != nil {
		sessionList = append(sessionList, user.Session)
	} else {
		sessionList = getSessionListBy(dsession.RoomID(this.RoomId))
	}
	for _, session := range sessionList {
		this.formatShowStatus(&s, session.UserName)
		sendNoticeToSession(session.ID, s)
	}
	this.SaveRoomDataToDb()
}

func (this *PiecePoint) IsValid() bool {
	return 0 <= this.Line && this.Line < 10 && 0 <= this.Column && this.Column < 9
}
