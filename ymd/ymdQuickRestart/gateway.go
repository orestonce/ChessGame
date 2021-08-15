package ymdQuickRestart

import (
	"github.com/google/uuid"
	"github.com/orestonce/ChessGame/ymd/ymdJson"
	"github.com/orestonce/ChessGame/ymd/ymdRedis"
	"log"
	"sync"
	"time"
)

type GatewayService struct {
	req         RedisInfo
	redisClient *ymdRedis.RedisClient
	locker      sync.Mutex
	sessionMap  map[string]*gatewaySession
}

type gatewaySession struct {
	runner      GatewaySessionRunner
	toClientMsg chan RedisExchange
}

func NewGatewayService(req RedisInfo) *GatewayService {
	redisClient := ymdRedis.NewRedisClient(req.RedisAddr)
	this := &GatewayService{
		req:         req,
		redisClient: redisClient,
		sessionMap:  map[string]*gatewaySession{},
	}
	for sessionId := range redisClient.MustHGetAll(req.GatewaySession()) {
		this.sendKickSession(sessionId)
	}
	go this.pullMessageFromRedis()
	return this
}

func (this *GatewayService) pullMessageFromRedis() {
	for {
		value, ok := this.redisClient.MustBLPop(1, this.req.GatewayIn())
		if !ok {
			continue
		}
		var msg RedisExchange
		ymdJson.MustUnmarshalFromString(value, &msg)
		switch msg.MsgType {
		case MTLogicKick:
			this.KickSession(msg.SessionId)
		case MTLogicWrite:
			this.WriteSession(msg.SessionId, msg)
		default:
			log.Println("GatewayService unkown message ", msg.MsgType)
		}
	}
}

type GatewaySessionRunner interface {
	WriteToClient(msg []byte) (err error)    // 多次调用
	ReadFromClient() (msg []byte, err error) // 多次调用
	CloseClient() (err error)                // 单次调用
}

func (this *GatewayService) getNextSessionId() string {
	return uuid.NewString()
}

func (this *GatewayService) NewSession(runner GatewaySessionRunner) {
	sessionId := this.getNextSessionId()
	log.Println(sessionId, "session open ", sessionId)

	toClientMsgCh := make(chan RedisExchange, 10)
	this.locker.Lock()
	this.sessionMap[sessionId] = &gatewaySession{
		runner:      runner,
		toClientMsg: toClientMsgCh,
	}
	this.locker.Unlock()
	defer this.KickSession(sessionId)

	quitWriteCh := make(chan struct{})
	closeOnce := sync.Once{}
	closeClient := func() {
		closeOnce.Do(func() {
			runner.CloseClient()
		})
	}
	this.redisClient.MustRPush(this.req.LogicIn(), ymdJson.MustMarshalToString(RedisExchange{
		MsgType:   MTGatewayNew,
		SessionId: sessionId,
	}))

	quitWriteFromReadCh := make(chan struct{})
	go func() {
		for {
			var msg RedisExchange
			var ok bool

			select {
			case msg, ok = <-toClientMsgCh:
			case <-time.After(time.Minute * 5):
			case <-quitWriteFromReadCh:
			}
			if !ok || msg.MsgType == MTLogicKick {
				log.Println(sessionId, "WriteToClient quit write. ", msg.MsgType)
				break
			}
			if msg.MsgType != MTLogicWrite {
				log.Println(sessionId, "WriteToClient unkown message type. ", msg.MsgType)
				continue
			}
			err := runner.WriteToClient(msg.MsgData)
			if err != nil {
				log.Println(sessionId, "WriteToClient error", err)
				break
			}
		}
		close(quitWriteCh)
		closeClient()
	}()

	quitReadCh := make(chan struct{})
	go func() {
		for {
			msgData, err := runner.ReadFromClient()
			if err != nil {
				log.Println(sessionId, "ReadFromClient error", err)
				break
			}
			this.redisClient.MustRPush(this.req.LogicIn(), ymdJson.MustMarshalToString(RedisExchange{
				MsgType:   MTGatewayRead,
				SessionId: sessionId,
				MsgData:   msgData,
			}))
		}
		close(quitReadCh)
		close(quitWriteFromReadCh)
		closeClient()
	}()

	<-quitReadCh
	<-quitWriteCh
}

func (this *GatewayService) KickSession(sessionId string) {
	this.locker.Lock()
	sessionObj := this.sessionMap[sessionId]
	delete(this.sessionMap, sessionId)
	if sessionObj != nil {
		close(sessionObj.toClientMsg)
	}
	this.locker.Unlock()

	if sessionObj != nil {
		this.sendKickSession(sessionId)
	}
}

func (this *GatewayService) sendKickSession(sessionId string) {
	this.redisClient.MustHDel(this.req.GatewaySession(), sessionId)
	this.redisClient.MustRPush(this.req.LogicIn(), ymdJson.MustMarshalToString(RedisExchange{
		MsgType:   MTGatewayClose,
		SessionId: sessionId,
	}))
	log.Println(sessionId, "kick session done.")
}

func (this *GatewayService) WriteSession(sessionId string, msg RedisExchange) {
	needKick := false
	this.locker.Lock()
	sessionObj := this.sessionMap[sessionId]
	if sessionObj == nil {
		log.Println(sessionId, "WriteSession 404")
	} else {
		select {
		case sessionObj.toClientMsg <- msg:
		default:
			needKick = true
		}
	}
	this.locker.Unlock()
	if needKick {
		this.KickSession(sessionId)
	}
}
