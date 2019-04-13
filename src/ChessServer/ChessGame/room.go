package ChessGame

import (
	"sync"
	"time"
	"ChessServer/ChessGame/ymdSync"
)

type GameRoomManager struct {
	core                *Game
	usernameToRoomIdMap map[string]string
	roomMap             map[string]*GameRoom
	locker              sync.RWMutex
	roomLocker          ymdSync.StringMutexMap
}

type GameRoom struct {
	mgr     *GameRoomManager
	roomId  string
	panel   GamePanel
	userMap map[string]*GameUser
}

func (this *GameRoom) IsEmpty() bool {
	return len(this.userMap) == 0
}

func (this *GameRoom) UserLeave(username string) {
	user := this.userMap[username]
	if user == nil {
		return
	}
	delete(this.userMap, username)
	var target *string
	if this.panel.UpperUsername == username {
		target = &this.panel.UpperUsername
	} else if this.panel.DownUsername == username {
		target = &this.panel.DownUsername
	} else {
		return
	}
	*target = ``
	if this.panel.IsGameRunning {
		this.panel.IsGameRunning = false
	}
	if this.panel.NextTurnUsername == username {
		this.panel.NextTurnUsername = ``
	}
	this.sync2Client(nil)
}

func (this *GameRoom) BroadcastToAll(a interface{}) {
	for _, user := range this.userMap {
		user.SendNotice(a)
	}
}

func (this *GameRoom) GetUser(username string) *GameUser {
	return this.userMap[username]
}

type ServerKickYou struct {
	ErrMsg string
}

func (this *GameRoomManager) OnUserLogin(user *GameUser) {
	for {
		this.locker.Lock()
		oldRoomId := this.usernameToRoomIdMap[user.username]

		if oldRoom := this.roomMap[oldRoomId]; oldRoomId != `` && oldRoom != nil {
			oldUser := oldRoom.GetUser(user.username)
			oldUser.SendNotice(ServerKickYou{
				ErrMsg: ErrNextLogin,
			})
			time.Sleep(time.Second)
			oldUser.conn.Close()
			this.locker.Unlock()
			oldUser.allExit.Wait()
		} else {
			break
		}
	}
	defer this.locker.Unlock()

	newRoom := this.roomMap[user.roomId]
	if newRoom == nil {
		newRoom = &GameRoom{
			mgr:     this,
			roomId:  user.roomId,
			userMap: map[string]*GameUser{},
		}
		newRoom.panel.Reset(newRoom)
		this.roomMap[user.roomId] = newRoom
	}
	newRoom.userMap[user.username] = user
	this.usernameToRoomIdMap[user.username] = newRoom.roomId
	newRoom.sync2Client(nil)
}

func (this *GameRoomManager) RemoveUser(username string) {
	this.locker.Lock()
	defer this.locker.Unlock()

	roomId := this.usernameToRoomIdMap[username]
	room := this.roomMap[roomId]
	if room == nil {
		return
	}
	room.panel.onUserLeave(username)
	delete(room.userMap, username)
	if room.IsEmpty() {
		delete(this.roomMap, roomId)
	}
	delete(this.usernameToRoomIdMap, username)
}
