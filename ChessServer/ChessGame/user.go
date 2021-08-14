package ChessGame

import (
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ymd/ymdJson"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"reflect"
	"strings"
)

type GameUser struct {
	Session *ent.DSession
}

func userHandleClientMessage(msg ymdQuickRestart.RedisExchange) {
	session := getSessionById(msg.SessionId)
	if session == nil {
		return
	}
	var callPacket CSExchangePacket
	ymdJson.MustUnmarshal(msg.MsgData, &callPacket)
	if !strings.HasPrefix(callPacket.Method, `Rpc`) || callPacket.PacketType != PtRpcCall {
		log.Println("Unknown call ", msg.SessionId)
		return
	}
	var reply interface{}
	if session.RoomID != `` && callPacket.Method != `RpcPing` {
		reply = GameRoomManagerHandleGameRoomPacket(session, callPacket.Method, callPacket.Data)
	} else {
		method := reflect.ValueOf(&GameUser{
			Session: session,
		}).MethodByName(callPacket.Method)
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
	gLogic.WriteSession(session.ID, ymdJson.MustMarshal(replyPacket))
}

func GameRoomManagerHandleGameRoomPacket(session *ent.DSession, methodName string, data []byte) (replyData interface{}) {
	room := getRoomById(session.RoomID)
	if room == nil {
		return nil
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
		reflect.ValueOf(session),
		request.Elem(),
	})
	return result[0].Interface()
}

func sendNoticeToSession(sessionId string, a interface{}) {
	var packet = CSExchangePacket{
		PacketType: PtRpcNotice,
		Method:     reflect.TypeOf(a).Name(),
		Data:       ymdJson.MustMarshal(a),
	}
	noticePacket := ymdJson.MustMarshal(packet)
	gLogic.WriteSession(sessionId, noticePacket)
}
