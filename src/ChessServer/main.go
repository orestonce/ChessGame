package main

import (
	"ChessServer/ChessGame"
	"flag"
)

func main() {
	pListenAddr := flag.String("l", "127.0.0.1:8912", "监听地址")
	pWsPath := flag.String("wsp", "/ChessGame", "websocket访问路径")
	pStorePath := flag.String("sp", "ChessGame", "数据库路径")
	flag.Parse()

	game := ChessGame.NewChessGame()
	game.InitGame(*pListenAddr, *pWsPath, *pStorePath)
	game.HandleRequests()
}