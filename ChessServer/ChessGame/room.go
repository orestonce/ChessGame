package ChessGame

import (
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"log"
)

type GameRoom struct {
	RoomId         string
	PanelFull      [LINE_COUNT][COLUMN_COUNT]GamePiece
	NextTurnUserID string
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
	err = gDbClient.DRoom.Update().Where(droom.ID(this.RoomId)).SetPanel(data.Panel).SetWUserID(data.WUserID).SetBUserID(data.BUserID).Exec(context.Background())
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
