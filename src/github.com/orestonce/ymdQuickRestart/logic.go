package ymdQuickRestart

import (
	"sync"
	"time"
	"github.com/orestonce/ymdRedis"
	"github.com/orestonce/ymdJson"
)

type LogicService struct {
	redisClient *ymdRedis.RedisClient
	req         RedisInfo
	msgChan     chan RedisExchange
	exitChan    chan struct{}
	exitOnce    sync.Once
}

func NewLogicService(req RedisInfo) *LogicService {
	redisClient := ymdRedis.NewRedisClient(req.RedisAddr)

	msgChan := make(chan RedisExchange)
	exitChan := make(chan struct{})
	this := &LogicService{
		redisClient: redisClient,
		req:         req,
		msgChan:     msgChan,
		exitChan:    exitChan,
	}
	wg := sync.WaitGroup{}
	wg.Add(2)
	go func() {
		var quit = false
		for !quit {
			value, ok := this.redisClient.MustBLPop(1, req.LogicIn())
			if ok {
				var data RedisExchange
				ymdJson.MustUnmarshalFromString(value, &data)
				this.msgChan <- data
			}
			select {
			case _, ok = <-exitChan:
				quit = true
			default:
			}
		}
		wg.Done()
	}()
	go func() {
		var quit = false
		ticker := time.NewTicker(time.Second)
		for !quit {
			select {
			case <-this.exitChan:
				quit = true
			case <-ticker.C:
				this.msgChan <- RedisExchange{
					MsgType: MTTimeout,
				}
			}
		}
		ticker.Stop()
		wg.Done()
	}()
	go func() {
		wg.Wait()
		close(this.msgChan)
	}()
	return this
}

func (this *LogicService) WriteSession(sessionId string, msgData []byte) {
	this.redisClient.MustRPush(this.req.GatewayIn(), ymdJson.MustMarshalToString(RedisExchange{
		SessionId: sessionId,
		MsgType:   MTLogicWrite,
		MsgData:   msgData,
	}))
}

func (this *LogicService) KickSession(sessionId string) {
	this.redisClient.MustRPush(this.req.GatewayIn(), ymdJson.MustMarshalToString(RedisExchange{
		SessionId: sessionId,
		MsgType:   MTLogicKick,
	}))
}

func (this *LogicService) Close() {
	this.exitOnce.Do(func() {
		close(this.exitChan)
	})
}

func (this *LogicService) GetMessageChan() (<-chan RedisExchange) {
	return this.msgChan
}

func (this *LogicService) GetMemoryState() (value string) {
	return this.redisClient.MustGetIgnoreNotExists(this.req.MemoryState())
}

func (this *LogicService) SetMemoryState(value string) {
	this.redisClient.MustSet(this.req.MemoryState(), value)
}
