package ChessGame

import (
	"github.com/gorilla/websocket"
	"strings"
	"reflect"
	"time"
	"sync"
	"ChessServer/ChessGame/ymdJson"
)

type GameUser struct {
	gameCore        *Game
	username        string
	roomId          string
	connWriteLocker sync.Mutex
	conn            *websocket.Conn
	noticeCh        chan []byte
	exitCh          chan struct{}
	allExit         sync.WaitGroup
}

func NewGameUser(gameCore *Game, conn *websocket.Conn) *GameUser {
	this := &GameUser{
		gameCore: gameCore,
		conn:     conn,
		noticeCh: make(chan []byte, 1024),
		exitCh:   make(chan struct{}),
	}
	return this
}

func (this *GameUser) HandleClientMessage(conn *websocket.Conn) {
	this.allExit.Add(2)
	defer this.allExit.Done()

	go func() {
		defer this.allExit.Done()
		for {
			select {
			case message := <-this.noticeCh:
				time.Sleep(time.Millisecond * 50)
				this.writeMessage(message)
				break
			case <-this.exitCh:
				return
			}
		}
	}()
	for {
		messageType, data, err := conn.ReadMessage()
		if err != nil {
			break
		}
		if messageType != websocket.BinaryMessage {
			continue
		}
		var callPacket CSExchangePacket
		ymdJson.MustUnmarshal(data, &callPacket)
		if !strings.HasPrefix(callPacket.Method, `Rpc`) || callPacket.PacketType != PtRpcCall {
			break
		}
		var reply interface{}
		if this.username != `` {
			reply = this.gameCore.gRoomManager.handleGameRoomPacket(this.username, callPacket.Method, callPacket.Data)
		} else {
			method := reflect.ValueOf(this).MethodByName(callPacket.Method)
			if !method.IsValid() {
				break
			}
			//fmt.Println(callPacket.Method, method.Type().NumIn(), method.Type().NumOut())
			request := reflect.New(method.Type().In(0)).Interface()
			ymdJson.MustUnmarshal(callPacket.Data, request)
			respValueList := method.Call([]reflect.Value{
				reflect.ValueOf(request).Elem(),
			})
			reply = respValueList[0].Interface()
		}
		var replyPacket CSExchangePacket
		replyPacket.Method = callPacket.Method
		replyPacket.PacketType = PtRpcReply
		replyPacket.Data = ymdJson.MustMarshal(reply)
		this.writeMessage(ymdJson.MustMarshal(replyPacket))
	}
}

func (this *GameRoomManager) handleGameRoomPacket(username, methodName string, data []byte) (replyData interface{}) {
	this.locker.RLock()
	defer this.locker.RUnlock()

	roomId := this.usernameToRoomIdMap[username]
	room := this.roomMap[roomId]
	if room == nil {
		return
	}
	// 同一room里的操作不要并发
	this.roomLocker.LockByString(roomId)
	defer this.roomLocker.UnlockByString(roomId)

	roomValue := reflect.ValueOf(room)
	method := roomValue.MethodByName(methodName)
	if !method.IsValid() || method.Type().NumIn() != 2 || method.Type().NumOut() != 1 {
		return
	}
	request := reflect.New(method.Type().In(1))
	ymdJson.MustUnmarshal(data, request.Interface())
	result := method.Call([]reflect.Value{
		reflect.ValueOf(username),
		request.Elem(),
	})
	return result[0].Interface()
}

func (this *GameUser) SendNotice(a interface{}) {
	var packet = CSExchangePacket{
		PacketType: PtRpcNotice,
		Method:     reflect.TypeOf(a).Name(),
		Data:       ymdJson.MustMarshal(a),
	}
	noticePacket := ymdJson.MustMarshal(packet)
	select {
	case this.noticeCh <- noticePacket:
	case <-this.exitCh:
	case <-time.After(time.Second):
	}

}

func (this *GameUser) writeMessage(message []byte) (err error) {
	this.connWriteLocker.Lock()
	err = this.conn.WriteMessage(websocket.BinaryMessage, message)
	this.connWriteLocker.Unlock()

	return err
}

func (this *GameUser) OnExit() {
	if this.roomId != `` {
		this.gameCore.gRoomManager.RemoveUser(this.username)
	}
	close(this.exitCh)
	this.conn.Close()
	this.allExit.Wait()
}
