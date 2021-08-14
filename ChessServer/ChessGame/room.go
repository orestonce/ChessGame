package ChessGame

import (
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"log"
	"time"
)

type GameRoomManager struct{}

type GameRoom struct {
	RoomId         string
	PanelFull      [LINE_COUNT][COLUMN_COUNT]GamePiece
	NextTurnUserId string
	Data           *ent.DRoom
}

func (this *GameRoom) IsEmpty() bool {
	return len(this.getSessionList()) == 0
}

func (this *GameRoom) UserLeave(session *ent.DSession) {
	this.onUserLeave(session.UserID)
	_, err := gDbClient.DSession.Delete().Where(dsession.ID(session.ID)).Exec(context.Background())
	if err != nil {
		log.Println("GameRoom.UserLeave", err)
	}
	data := this.Data
	err = gDbClient.DRoom.Update().Where(droom.ID(this.RoomId)).SetPanel(data.Panel).SetUpUserID(data.UpUserID).SetDownUserID(data.DownUserID).Exec(context.Background())
	if err != nil {
		log.Println("GamePanel onUserLeave", err)
	}
}

func (this *GameRoom) BroadcastToAll(a interface{}) {
	for _, session := range this.getSessionList() {
		sendNoticeToSession(session.ID, a)
	}
}

type ServerKickYou struct {
	ErrMsg string
}

func (this *Game) OnUserLogin(user *GameUser) {
	// 踢掉上一个用户
	{
		for _, oldSession := range getSessionListBy(dsession.UserID(user.Session.UserID), dsession.IDNEQ(user.Session.ID)) {
			log.Println("kickSession", oldSession, user.Session)
			sendNoticeToSession(oldSession.ID, ServerKickYou{
				ErrMsg: ErrNextLogin,
			})
			this.kickSession(oldSession.ID)
		}
	}
	room := getRoomById(user.Session.RoomID)
	if room == nil {
		room = &GameRoom{
			RoomId: user.Session.RoomID,
			Data: &ent.DRoom{
				ID:         user.Session.RoomID,
				CreateTime: time.Now(),
			},
		}
		room.LoadPanelFromData()
		err := gDbClient.DRoom.Create().SetID(user.Session.RoomID).SetCreateTime(room.Data.CreateTime).Exec(context.Background())
		if err != nil {
			log.Println("GameRoomManager.OnUserLogin", err)
		}
	}
	room.sync2Client(nil)
}
