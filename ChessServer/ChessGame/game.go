package ChessGame

import (
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"github.com/orestonce/ChessGame/ymd/ymdJson"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"github.com/syndtr/goleveldb/leveldb"
	"github.com/syndtr/goleveldb/leveldb/storage"
	"log"
	"time"
)

type Game struct {
	db           *leveldb.DB
	gRoomManager *GameRoomManager
	logic        *ymdQuickRestart.LogicService
}

func NewChessGame(storePath string, logic *ymdQuickRestart.LogicService) *Game {
	this := &Game{}
	store, err := storage.OpenFile(storePath, false)
	if err != nil {
		log.Fatal("InitGame OpenLevelDBStorage", err)
	}
	this.db, err = leveldb.Open(store, nil)
	if err != nil {
		log.Fatal("InitGame OpenLevelDB", err)
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

func (this *Game) getPassword(username string) (password string) {
	value, err := this.db.Get([]byte(username), nil)
	if err == leveldb.ErrNotFound {
		err = nil
	}
	if err != nil {
		panic(`GetPassword ` + err.Error())
	}
	return string(value)
}

func (this *Game) setPassword(username string, password string) {
	err := this.db.Put([]byte(username), []byte(password), nil)
	if err != nil {
		panic(`SetPassword ` + err.Error())
	}
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
