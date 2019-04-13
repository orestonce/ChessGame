package ChessGame

import (
	"net/http"
	"github.com/gorilla/websocket"
	"time"
	"log"
	"encoding/json"
	"net"
	"github.com/syndtr/goleveldb/leveldb/storage"
	"github.com/syndtr/goleveldb/leveldb"
	"ChessServer/ChessGame/ymdSync"
	"ChessServer/ChessGame/ymdError"
)

type Game struct {
	mux          *http.ServeMux
	ln           net.Listener
	db           *leveldb.DB
	gUserLocker  ymdSync.StringMutexMap
	gRoomManager *GameRoomManager
	local        *time.Location
}

func NewChessGame() *Game {
	return &Game{}
}

func (this *Game) InitGame(listenAddr string, wsPath string, storePath string) {
	this.mux = http.NewServeMux()
	this.mux.HandleFunc(wsPath, func(w http.ResponseWriter, r *http.Request) {
		upgreader := websocket.Upgrader{
			HandshakeTimeout:  time.Second * 10,
			ReadBufferSize:    2 * 1024,
			WriteBufferSize:   4 * 1024,
			EnableCompression: true,
		}
		conn, err := upgreader.Upgrade(w, r, nil)
		if err != nil {
			log.Println(err)
			return
		}

		user := NewGameUser(this, conn)

		err, stack := ymdError.PanicToErrorAndStack(func() {
			user.HandleClientMessage(conn)
		})
		if err != nil {
			log.Println(err.Error(), stack.String())
		}
		user.OnExit()
	})
	var err error
	this.ln, err = net.Listen(`tcp`, listenAddr)
	if err != nil {
		log.Fatalln("InitGame ListenTcpAddr", err)
	}
	store, err := storage.OpenFile(storePath, false)
	if err != nil {
		log.Fatal("InitGame OpenLevelDBStorage", err)
	}
	this.db, err = leveldb.Open(store, nil)
	if err != nil {
		log.Fatal("InitGame OpenLevelDB", err)
	}
	log.Println("start listen on ", listenAddr)

	this.gRoomManager = &GameRoomManager{
		core:                this,
		usernameToRoomIdMap: map[string]string{},
		roomMap:             map[string]*GameRoom{},
	}
	local, err := time.LoadLocation("Asia/Chongqing")
	if err != nil {
		log.Fatalln("InitGame loadLocation", err)
	}
	this.local = local
}

func (this *Game) getPassword(username string) (password string) {
	value, err := this.db.Get([]byte(username), nil)
	if err == leveldb.ErrNotFound {
		err = nil
	}
	if err != nil {
		panic(`GetPassword ` + err.Error())
	}
	return string(value)
}

func (this *Game) setPassword(username string, password string) {
	err := this.db.Put([]byte(username), []byte(password), nil)
	if err != nil {
		panic(`SetPassword ` + err.Error())
	}
}

type ClientRequestCommon struct {
	Method string          `json:"method"`
	Data   json.RawMessage `json:"data"`
}

func (this *Game) HandleRequests() {
	http.Serve(this.ln, this.mux)
}
