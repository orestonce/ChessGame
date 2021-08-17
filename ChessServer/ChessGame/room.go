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
	winUserId := ``
	if this.Data.BUserID == session.UserID {
		this.Data.BUserID = ``
		winUserId = this.Data.WUserID
	} else if this.Data.WUserID == session.UserID {
		this.Data.WUserID = ``
		winUserId = this.Data.BUserID
	}
	_, err := gDbClient.DSession.Delete().Where(dsession.ID(session.ID)).Exec(context.Background())
	if err != nil {
		log.Println("GameRoom.UserLeave", err)
		return
	}
	data := this.Data
	err = gDbClient.DRoom.Update().Where(droom.ID(this.RoomId)).SetPanel(data.Panel).SetWUserID(data.WUserID).SetBUserID(data.BUserID).Exec(context.Background())
	if err != nil {
		log.Println("GamePanel onUserLeave", err)
		return
	}
	if this.Data.IsGameRunning {
		this.onGameOver(winUserId)
	} else {
		this.sync2Client(nil)
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
