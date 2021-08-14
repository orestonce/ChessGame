package ChessGame

import (
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"github.com/orestonce/ChessGame/ent/duser"
	"github.com/orestonce/ChessGame/ent/predicate"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"time"
)

var gLogic *ymdQuickRestart.LogicService

func insertSession(sessionId string) {
	err := gDbClient.DSession.Create().SetID(sessionId).SetCreateTime(time.Now()).Exec(context.Background())
	if err != nil {
		log.Println("insertSession error", err)
	}
}

func kickSession(sessionId string) {
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
