package ChessGame

import (
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"github.com/orestonce/ChessGame/ent/duser"
	"github.com/orestonce/ChessGame/ent/predicate"
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"time"
)

type Game struct{}

var gLogic *ymdQuickRestart.LogicService
var gGame *Game

func InitChessGame(mysqlSchema string, logic *ymdQuickRestart.LogicService) *Game {
	gGame = &Game{}
	var err error
	gDbClient, err = ent.Open("mysql", mysqlSchema)
	if err != nil {
		log.Fatal("InitChessGame ent.Open", err)
		return nil
	}
	err = gDbClient.Schema.Create(context.Background())
	if err != nil {
		log.Println("InitChessGame Schema.Create", err)
		return nil
	}
	gLogic = logic
	return gGame
}

func (this *Game) RunMainLogic() {
	this.loadMemoryState()
	this.processLogicMessage()
	this.saveMemoryState()
}

func (this *Game) loadMemoryState() {
	start := time.Now()
	log.Println(`loadMemoryState`, time.Since(start))
}

func (this *Game) processLogicMessage() {
	for msg := range gLogic.GetMessageChan() {
		errMsg, stack := ymdError.PanicToErrMsgAndStack(func() {
			switch msg.MsgType {
			case ymdQuickRestart.MTGatewayNew:
				insertSession(msg.SessionId)
			case ymdQuickRestart.MTGatewayRead:
				userHandleClientMessage(msg)
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

func insertSession(sessionId string) {
	err := gDbClient.DSession.Create().SetID(sessionId).SetCreateTime(time.Now()).Exec(context.Background())
	if err != nil {
		log.Println("insertSession error", err)
	}
}

func (this *Game) saveMemoryState() {
	start := time.Now()
	log.Println(`saveMemoryState`, time.Since(start))
}

func (this *Game) kickSession(sessionId string) {
	session := getSessionById(sessionId)
	if session == nil {
		return
	}
	_, err := gDbClient.DSession.Delete().Where(dsession.ID(sessionId)).Exec(context.Background())
	if err != nil {
		log.Println("Game.kickSession", err, sessionId)
		return
	}
	room := getRoomById(session.RoomID)
	if room != nil {
		room.UserLeave(session)
		if room.IsEmpty() {
			_, err = gDbClient.DRoom.Delete().Where(droom.ID(room.RoomId)).Exec(context.Background())
			if err != nil {
				log.Println("Game.kickSession", err)
			}
		}
	}
	gLogic.KickSession(sessionId)
}

func getRoomById(id string) *GameRoom {
	room, err := gDbClient.DRoom.Query().Where(droom.ID(id)).Only(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); !ok {
			log.Println("userHandleClientMessage ", err)
		}
		return nil
	}
	r := &GameRoom{
		RoomId: id,
		Data:   room,
	}
	r.LoadPanelFromData()
	return r
}

func getSessionListBy(pAnd ...predicate.DSession) []*ent.DSession {
	sessionList, err := gDbClient.DSession.Query().Where(dsession.And(pAnd...)).All(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); !ok {
			log.Println("userHandleClientMessage ", err)
		}
		return nil
	}
	return sessionList
}

func getSessionById(sessionId string) *ent.DSession {
	session, err := gDbClient.DSession.Query().Where(dsession.ID(sessionId)).Only(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); !ok {
			log.Println("getSessionById ", err)
		}
		return nil
	}
	return session
}

func getUserById(id string) *ent.DUser {
	user, err := gDbClient.DUser.Query().Where(duser.ID(id)).Only(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); !ok {
			log.Println("getUserById ", err)
		}
		return nil
	}
	return user
}

func getUserNameByIdIgnoreEmpty(id string) string {
	if id == "" {
		return ""
	}
	u := getUserById(id)
	if u == nil {
		return ""
	}
	return u.Name
}

func getUserByName(name string) *ent.DUser {
	user, err := gDbClient.DUser.Query().Where(duser.Name(name)).Only(context.Background())
	if err != nil {
		if _, ok := err.(*ent.NotFoundError); !ok {
			log.Println("getUserByName ", err)
		}
		return nil
	}
	return user
}
