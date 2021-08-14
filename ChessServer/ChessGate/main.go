package ChessGate

import (
	"github.com/gorilla/websocket"
	"github.com/orestonce/ChessGame/ymd/ymdQuickRestart"
	"log"
	"net"
	"net/http"
	"time"
)

func RunChessGate(laddr string, redis ymdQuickRestart.RedisInfo, wspath string, pnginx bool) {
	ln, err := net.Listen("tcp", laddr)
	if err != nil {
		log.Fatal("Cannot listen to", laddr, err)
	}
	log.Println("listen success", laddr)
	mux := http.NewServeMux()
	gateway := ymdQuickRestart.NewGatewayService(redis)
	mux.HandleFunc(wspath, func(writer http.ResponseWriter, request *http.Request) {
		upgreader := websocket.Upgrader{
			HandshakeTimeout:  time.Second * 10,
			ReadBufferSize:    2 * 1024,
			WriteBufferSize:   4 * 1024,
			EnableCompression: true,
		}
		conn, err := upgreader.Upgrade(writer, request, nil)
		if err != nil {
			log.Println(err)
			return
		}
		clientIp, _, _ := net.SplitHostPort(request.RemoteAddr)
		if pnginx {
			clientIp = request.Header.Get(`X-Real-IP`)
		}
		gateway.NewSession(ChessGateClient{
			ws: conn,
		}, clientIp)
		conn.Close()
	})
	http.Serve(ln, mux)
}

type ChessGateClient struct {
	ws *websocket.Conn
}

func (this ChessGateClient) WriteToClient(msg []byte) (err error) {
	this.ws.SetWriteDeadline(time.Now().Add(time.Second * 10))
	return this.ws.WriteMessage(websocket.BinaryMessage, msg)
}

func (this ChessGateClient) ReadFromClient() (msg []byte, err error) {
	this.ws.SetReadLimit(1024 * 10) // 10KB
	_, msg, err = this.ws.ReadMessage()
	return
}

func (this ChessGateClient) CloseClient() (err error) {
	return this.ws.Close()
}
