package ChessCore

import (
	"github.com/orestonce/ChessGame/ymd/ymdJson"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"reflect"
	"strings"
)

type GameUser struct {
	gameCore  *Game `json:"-"`
	SessionId string
	Username  string
	RoomId    string
}

func (this *GameUser) HandleClientMessage(msg ymdQuickRestart.RedisExchange) {
	var callPacket CSExchangePacket
	ymdJson.MustUnmarshal(msg.MsgData, &callPacket)
	if !strings.HasPrefix(callPacket.Method, `Rpc`) || callPacket.PacketType != PtRpcCall {
		log.Println("Unkown call ", msg.SessionId)
		return
	}
	var reply interface{}
	if this.RoomId != `` && callPacket.Method != `RpcPing` {
		reply = this.gameCore.gRoomManager.handleGameRoomPacket(this.RoomId, this.Username, callPacket.Method, callPacket.Data)
	} else {
		method := reflect.ValueOf(this).MethodByName(callPacket.Method)
		if !method.IsValid() {
			log.Println("Unkown method", callPacket.Method)
			return
		}
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

func (this *GameRoomManager) handleGameRoomPacket(roomId, username, methodName string, data []byte) (replyData interface{}) {
	room := this.RoomMap[roomId]
	if room == nil {
		return
	}
	roomValue := reflect.ValueOf(room)
	method := roomValue.MethodByName(methodName)
	if !method.IsValid() || method.Type().NumIn() != 2 || method.Type().NumOut() != 1 {
		log.Println("Method invalid", methodName)
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
	this.writeMessage(noticePacket)
}

func (this *GameUser) writeMessage(message []byte) {
	this.gameCore.logic.WriteSession(this.SessionId, message)
}
