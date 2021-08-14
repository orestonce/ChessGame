package ChessCore

import (
	"github.com/orestonce/ChessGame/ymd/ymdTime"
	"time"
)

const (
	ErrUsernameExists          = `用户名已注册`
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
	password := this.gameCore.getPassword(req.Username)
	if password == `` {
		this.gameCore.setPassword(req.Username, req.Password)
	} else {
		resp.ErrMsg = ErrUsernameExists
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
	password := this.gameCore.getPassword(req.Username)
	if password == `` || password != req.Password || req.Username == `` {
		resp.ErrMsg = ErrPassword
		return
	}
	if req.RoomId == `` {
		resp.ErrMsg = ErrRoomIdInvalid
		return
	}
	if this.Username != `` || this.RoomId != `` {
		resp.ErrMsg = ErrLoginRepeat
		return
	}
	this.Username = req.Username
	this.RoomId = req.RoomId
	this.gameCore.gRoomManager.OnUserLogin(this)
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

func (this *GameRoom) RpcChat(username string, req ChatRequest) (resp ChatResponse) {
	message := ChatMessage{
		TimeStr:  ymdTime.DefaultFormat(time.Now()),
		Username: username,
		Text:     req.Text,
	}
	for _, user := range this.UserMap {
		user.SendNotice(message)
	}
	return
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

func (this *GameRoom) RpcTakeSite(username string, req TakeSiteRequest) (resp TakeSiteResponse) {
	panel := &this.Panel
	if panel.IsGameRunning {
		resp.ErrMsg = ErrGameIsRunning
		return
	}

	if panel.UpperUsername == `` && panel.DownUsername != username {
		panel.UpperUsername = username
	} else if panel.DownUsername == `` && panel.UpperUsername != username {
		panel.DownUsername = username
	} else {
		resp.ErrMsg = ErrNoPosition
		return
	}
	if panel.DownUsername != `` && panel.UpperUsername != `` {
		panel.IsGameRunning = true
		panel.NextTurnUsername = panel.DownUsername
		panel.LoadNormalPanelFull()
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

func (this *GameRoom) RpcMovePiece(username string, req MovePieceRequest) (resp MovePieceResponse) {
	if !this.Panel.IsGameRunning {
		resp.ErrMsg = ErrGameIsNotRunning
		return
	}
	if !this.Panel.CanMove(username, req.FromPoint, req.ToPoint) {
		resp.ErrMsg = ErrCannotMove
		return
	}
	this.Panel.DoMove(req.FromPoint, req.ToPoint)
	return
}

type GetSuggestionRequest struct {
	FromPoint PiecePoint
}

type GetSuggestionResponse struct {
	FromPoint     PiecePoint
	CanMoveToList []PiecePoint
}

func (this *GameRoom) RpcGetSuggestion(username string, req GetSuggestionRequest) (resp GetSuggestionResponse) {
	for line := int32(0); line < LINE_COUNT; line++ {
		for column := int32(0); column < COLUMN_COUNT; column++ {
			to := PiecePoint{Line: line, Column: column}
			if this.Panel.CanMove(username, req.FromPoint, to) {
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

func (this *GameRoom) RpcReGame(username string, req ReGameRequest) (resp ReGameResponse) {
	panel := &this.Panel
	if panel.IsGameRunning {
		resp.ErrMsg = ErrGameIsRunning
		return
	}
	if panel.DownUsername == `` || panel.UpperUsername == `` {
		resp.ErrMsg = ErrUserNotEnough
		return
	}
	panel.IsGameRunning = true
	panel.LoadNormalPanelFull()
	panel.NextTurnUsername = panel.UpperUsername
	this.sync2Client(nil)
	return
}

func (this *GameRoom) sync2Client(user *GameUser) {
	var s SyncPanelMessage
	this.Panel.formatPanelFull(&s)
	var userMap map[string]*GameUser
	if user != nil {
		userMap = map[string]*GameUser{
			user.Username: user,
		}
	} else {
		userMap = this.UserMap
	}
	for _, user := range userMap {
		this.Panel.formatShowStatus(&s, user.Username)
		user.SendNotice(s)
	}
}

func (this *PiecePoint) IsValid() bool {
	return 0 <= this.Line && this.Line < 10 && 0 <= this.Column && this.Column < 9
}
