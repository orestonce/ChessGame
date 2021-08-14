package ChessGame

import (
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"github.com/orestonce/ChessGame/ymd/ymdJson"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"time"
)

type Game struct {
	gRoomManager *GameRoomManager
	logic        *ymdQuickRestart.LogicService
}

func InitChessGame(mysql_schema string, logic *ymdQuickRestart.LogicService) *Game {
	this := &Game{}
	var err error
	gDbClient, err = ent.Open("mysql", mysql_schema)
	if err != nil {
		log.Fatal("InitChessGame ent.Open", err)
		return nil
	}
	err = gDbClient.Schema.Create(context.Background())
	if err != nil {
		log.Println("InitChessGame Schema.Create", err)
		return nil
	}
	this.gRoomManager = &GameRoomManager{
		core:                   this,
		SessionIdToGameUserMap: map[string]*GameUser{},
		UsernameToSessionIdMap: map[string]string{},
		RoomMap:                map[string]*GameRoom{},
	}
	this.logic = logic
	return this
}

func (this *Game) RunMainLogic() {
	this.loadMemoryState()
	this.processLogicMessage()
	this.saveMemoryState()
}

func (this *Game) loadMemoryState() {
	start := time.Now()
	value := this.logic.GetMemoryState()
	if value != `` {
		ymdJson.MustUnmarshalFromString(value, this.gRoomManager)
		this.gRoomManager.core = this
		for _, user := range this.gRoomManager.SessionIdToGameUserMap {
			user.gameCore = this
		}
		for _, room := range this.gRoomManager.RoomMap {
			room.mgr = this.gRoomManager
			for _, user := range room.UserMap {
				user.gameCore = this
			}
			room.Panel.room = room
		}
	}
	log.Println(`loadMemoryState`, time.Since(start))
}

func (this *Game) processLogicMessage() {
	for msg := range this.logic.GetMessageChan() {
		errMsg, stack := ymdError.PanicToErrMsgAndStack(func() {
			switch msg.MsgType {
			case ymdQuickRestart.MTGatewayNew:
				this.gRoomManager.SessionIdToGameUserMap[msg.SessionId] = &GameUser{
					gameCore:  this,
					SessionId: msg.SessionId,
				}
			case ymdQuickRestart.MTGatewayRead:
				user := this.gRoomManager.SessionIdToGameUserMap[msg.SessionId]
				if user == nil {
					break
				}
				user.HandleClientMessage(msg)
			case ymdQuickRestart.MTGatewayClose:
				this.kickSession(msg.SessionId)
			}
		})
		if errMsg != `` {
			this.kickSession(msg.SessionId)
			log.Println(msg.SessionId, "Unkown error", errMsg, stack)
		}
	}
}

func (this *Game) saveMemoryState() {
	start := time.Now()
	value := ymdJson.MustMarshalToString(this.gRoomManager)
	this.logic.SetMemoryState(value)
	log.Println(`saveMemoryState`, time.Since(start))
}

func (this *Game) kickSession(sessionId string) {
	user := this.gRoomManager.SessionIdToGameUserMap[sessionId]
	if user == nil {
		return
	}
	delete(this.gRoomManager.SessionIdToGameUserMap, sessionId)
	delete(this.gRoomManager.UsernameToSessionIdMap, user.Username)
	room := this.gRoomManager.RoomMap[user.RoomId]
	if room != nil {
		room.UserLeave(user.Username)
		if room.IsEmpty() {
			delete(this.gRoomManager.RoomMap, user.RoomId)
		}
	}
	this.logic.KickSession(sessionId)
}
