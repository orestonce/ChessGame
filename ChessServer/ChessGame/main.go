package ChessGame

import (
	_ "github.com/go-sql-driver/mysql"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"os"
	"os/signal"
	"syscall"
)

var gDbClient *ent.Client

func RunChessGame(redis ymdQuickRestart.RedisInfo, mysql_schema string) {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	logic := ymdQuickRestart.NewLogicService(redis)
	go func() {
		sigCh := make(chan os.Signal, 1)
		signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
		sig := <-sigCh

		log.Println("Logic close with signal", sig.String())
		logic.Close()
	}()
	game := InitChessGame(mysql_schema, logic)
	log.Println("start game ...")
	game.RunMainLogic()
}
