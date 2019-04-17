package main

import (
	"flag"
	"github.com/orestonce/ymdQuickRestart"
	"ChessServer/ChessGame/ChessCore"
	"log"
	"os"
	"os/signal"
	"syscall"
)

func main() {
	var redis ymdQuickRestart.RedisInfo
	var spath string
	flag.StringVar(&redis.RedisAddr, `raddr`, `127.0.0.1:6379`, `redis地址`)
	flag.StringVar(&redis.Prefix, `rprefix`, `chess`, `redis前缀`)
	flag.StringVar(&spath, `spath`, `ChessGame`, `数据储存位置`)
	flag.Parse()

	log.SetFlags(log.LstdFlags | log.Lshortfile)
	logic := ymdQuickRestart.NewLogicService(redis)
	go func() {
		sigCh := make(chan os.Signal, 1)
		signal.Notify(sigCh, syscall.SIGINT, syscall.SIGTERM)
		sig := <-sigCh

		log.Println("Logic close with signal", sig.String())
		logic.Close()
	}()
	game := ChessCore.NewChessGame(spath, logic)
	game.RunMainLogic()
}
