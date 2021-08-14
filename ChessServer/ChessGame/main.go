package ChessGame

import (
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"os"
	"os/signal"
	"syscall"
)

func RunChessGame(redis ymdQuickRestart.RedisInfo, spath string) {
	log.SetFlags(log.LstdFlags | log.Lshortfile)
	logic := ymdQuickRestart.NewLogicService(redis)
	go func() {
		sigCh := make(chan os.Signal, 1)
		signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
		sig := <-sigCh

		log.Println("Logic close with signal", sig.String())
		logic.Close()
	}()
	game := NewChessGame(spath, logic)
	log.Println("start game ...")
	game.RunMainLogic()
}
