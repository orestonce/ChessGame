package ymdQuickRestart

import (
	"testing"
	"io"
	"time"
	"github.com/orestonce/ymdAssert"
	"reflect"
	"log"
	"github.com/orestonce/ymdRedis"
)

var gRedisInfo = RedisInfo{
	RedisAddr: `192.168.56.111:6379`,
	Prefix:    `qrtest`,
}

func TestGatewayService_NewSession(t *testing.T) {
	client := ymdRedis.NewRedisClient(gRedisInfo.RedisAddr)
	client.MustFlushAll()

	gate := NewGatewayService(gRedisInfo)

	oldReadList := []string{
		"Register",
		"Login",
	}
	session := &FakeGateSession{
		readList: oldReadList,
		closeCh:  make(chan struct{}),
	}
	logic := NewLogicService(gRedisInfo)
	go func() {
		<-session.closeCh
		logic.Close()
	}()
	go func() {
		gate.NewSession(session, ``)
	}()
	for msg := range logic.GetMessageChan() {
		log.Println(msg.MsgType, msg.SessionId)
		if msg.MsgType == MTGatewayRead {
			logic.WriteSession(msg.SessionId, msg.MsgData)
		}
	}
	ymdAssert.True(reflect.DeepEqual(session.writeList, oldReadList), session.writeList, oldReadList)
}

type FakeGateSession struct {
	readList  []string
	writeList []string
	closeCh   chan struct{}
}

func (this *FakeGateSession) CloseClient() error {
	close(this.closeCh)
	return nil
}

func (this *FakeGateSession) WriteToClient(msg []byte) (err error) {
	this.writeList = append(this.writeList, string(msg))
	return
}
func (this *FakeGateSession) ReadFromClient() (msg []byte, err error) {
	if len(this.readList) > 0 {
		msg = []byte(this.readList[0])
		this.readList = this.readList[1:]
	} else {
		time.Sleep(time.Second)
		err = io.EOF
	}
	return
}
