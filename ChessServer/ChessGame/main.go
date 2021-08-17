package ChessGame

import (
	"context"
	_ "github.com/go-sql-driver/mysql"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"os"
	"os/signal"
	"syscall"
)

var gDbClient *ent.Client

func RunChessGame(redis ymdQuickRestart.RedisInfo, mysql_schema string) {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	gLogic = ymdQuickRestart.NewLogicService(redis)
	go func() {
		sigCh := make(chan os.Signal, 1)
		signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
		sig := <-sigCh

		log.Println("Logic close with signal", sig.String())
		gLogic.Close()
	}()
	var err error
	gDbClient, err = ent.Open("mysql", mysql_schema)
	if err != nil {
		log.Fatal("InitChessGame ent.Open", err)
		return
	}
	err = gDbClient.Schema.Create(context.Background())
	if err != nil {
		log.Println("InitChessGame Schema.Create", err)
		return
	}
	log.Println("start game ok")
	for msg := range gLogic.GetMessageChan() {
		errMsg, stack := ymdError.PanicToErrMsgAndStack(func() {
			switch msg.MsgType {
			case ymdQuickRestart.MTGatewayNew:
				insertSession(msg.SessionId)
			case ymdQuickRestart.MTGatewayRead:
				userHandleClientMessage(msg)
			case ymdQuickRestart.MTGatewayClose:
				kickSessionById(msg.SessionId)
			}
		})
		if errMsg != `` {
			kickSessionById(msg.SessionId)
			log.Println(msg.SessionId, "Unknown error", errMsg, stack)
		}
	}
}
