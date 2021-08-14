package ChessCore

type GameRoomManager struct {
	core                   *Game `json:"-"`
	SessionIdToGameUserMap map[string]*GameUser
	UsernameToSessionIdMap map[string]string
	RoomMap                map[string]*GameRoom
}

type GameRoom struct {
	mgr     *GameRoomManager `json:"-"`
	RoomId  string
	Panel   GamePanel
	UserMap map[string]*GameUser
}

func (this *GameRoom) IsEmpty() bool {
	return len(this.UserMap) == 0
}

func (this *GameRoom) UserLeave(username string) {
	user := this.UserMap[username]
	if user == nil {
		return
	}
	delete(this.UserMap, username)
	this.Panel.onUserLeave(username)
}

func (this *GameRoom) BroadcastToAll(a interface{}) {
	for _, user := range this.UserMap {
		user.SendNotice(a)
	}
}

type ServerKickYou struct {
	ErrMsg string
}

func (this *GameRoomManager) OnUserLogin(user *GameUser) {
	// 踢掉上一个用户
	{
		oldSessionId := this.UsernameToSessionIdMap[user.Username]
		oldUser := this.SessionIdToGameUserMap[oldSessionId]
		if oldSessionId != `` {
			oldUser.SendNotice(ServerKickYou{
				ErrMsg: ErrNextLogin,
			})
			this.core.kickSession(oldSessionId)
		}
	}

	newRoom := this.RoomMap[user.RoomId]
	if newRoom == nil {
		newRoom = &GameRoom{
			mgr:     this,
			RoomId:  user.RoomId,
			UserMap: map[string]*GameUser{},
		}
		newRoom.Panel.Reset(newRoom)
		this.RoomMap[user.RoomId] = newRoom
	}
	newRoom.UserMap[user.Username] = user
	this.UsernameToSessionIdMap[user.Username] = user.SessionId
	this.SessionIdToGameUserMap[user.SessionId] = user
	this.RoomMap[user.RoomId] = newRoom
	newRoom.sync2Client(nil)
}
