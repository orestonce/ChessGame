package main

import (
	"github.com/orestonce/ChessGame/ChessServer/ChessGame"
	"github.com/orestonce/ChessGame/ChessServer/ChessGate"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"github.com/spf13/cobra"
)

var root = cobra.Command{
	Use: "ChessServer",
}

func init() {
	var redis ymdQuickRestart.RedisInfo
	var mysql_schema string
	game := &cobra.Command{
		Use: "Game",
		Run: func(cmd *cobra.Command, args []string) {
			ChessGame.RunChessGame(redis, mysql_schema)
		},
	}
	game.Flags().StringVarP(&redis.RedisAddr, `raddr`, ``, `127.0.0.1:6379`, `redis地址`)
	game.Flags().StringVarP(&redis.Prefix, `rprefix`, ``, `chess`, `redis前缀`)
	game.Flags().StringVarP(&mysql_schema, `mysql_schema`, ``, `root:@tcp(127.0.0.1:3306)/chess?charset=utf8`, `mysql连接`)
	root.AddCommand(game)

	var laddr string
	var wspath string
	var pnginx bool

	gate := &cobra.Command{
		Use: "Gate",
		Run: func(cmd *cobra.Command, args []string) {
			ChessGate.RunChessGate(laddr, redis, wspath, pnginx)
		},
	}
	gate.Flags().StringVarP(&redis.RedisAddr, `raddr`, ``, `127.0.0.1:6379`, `redis地址`)
	gate.Flags().StringVarP(&redis.Prefix, `rprefix`, ``, `chess`, `redis前缀`)
	gate.Flags().StringVarP(&laddr, `laddr`, ``, `127.0.0.1:8912`, `监听地址`)
	gate.Flags().StringVarP(&wspath, `wspath`, ``, `/ChessGame`, `websocket路径`)
	gate.Flags().BoolVarP(&pnginx, `pnginx`, ``, false, `是否使用了nginx作为前端代理`)
	root.AddCommand(gate)
}

func main() {
	if err := root.Execute(); err != nil {
		root.Help()
	}
}
