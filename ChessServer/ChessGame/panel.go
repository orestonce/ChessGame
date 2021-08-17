package ChessGame

import (
	"bytes"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/dsession"
	"log"
	"strings"
	"unicode"
)

const (
	MAX_VALUE_Y = 10
	MAX_VALUE_X = 9
)

type GamePiece byte

func (this *GameRoom) LoadPanelFromData() {
	if this.Data == nil || this.Data.Panel == "" {
		this.PanelFull = [MAX_VALUE_Y][MAX_VALUE_X]GamePiece{
			{'r', 'n', 'b', 'a', 'k', 'a', 'b', 'n', 'r'},
			{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
			{'0', 'c', '0', '0', '0', '0', '0', 'c', '0'},
			{'p', '0', 'p', '0', 'p', '0', 'p', '0', 'p'},
			{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
			{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
			{'P', '0', 'P', '0', 'P', '0', 'P', '0', 'P'},
			{'0', 'C', '0', '0', '0', '0', '0', 'C', '0'},
			{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
			{'R', 'N', 'B', 'A', 'K', 'A', 'B', 'N', 'R'},
		}
	} else {
		lineData := strings.Split(this.Data.Panel, "/")
		for y := 0; y < MAX_VALUE_Y; y++ {
			r := strings.NewReader(lineData[y])
			for x := 0; x < MAX_VALUE_X; {
				ch, err := r.ReadByte()
				if err != nil {
					log.Println("Error panel data", this.Data.Panel, lineData[y], err)
					return
				}
				if '0' < ch && ch <= '9' {
					emptyCnt := int(ch - '0')
					for emptyCnt > 0 && x < MAX_VALUE_X {
						this.PanelFull[y][x] = '0'
						x++
						emptyCnt--
					}
				} else {
					this.PanelFull[y][x] = GamePiece(ch)
					x++
				}
			}
		}
		ch := this.Data.Panel[len(this.Data.Panel)-1]
		if ch == 'w' {
			this.NextTurnUserID = this.Data.WUserID
		} else if ch == 'b' {
			this.NextTurnUserID = this.Data.BUserID
		} else {
			log.Println("error next user id data", this.Data.Panel)
		}
	}
}

func (this *GameRoom) CanMove(session *ent.DSession, from PiecePoint, to PiecePoint) bool {
	fromPiece := this.getPiece(from)
	toPiece := this.getPiece(to)

	if !from.IsValid() || !to.IsValid() {
		return false
	}
	if this.NextTurnUserID != session.UserID {
		return false
	}

	if (fromPiece.IsW() && this.NextTurnUserID != this.Data.WUserID) || (fromPiece.IsB() && this.NextTurnUserID != this.Data.BUserID) {
		return false
	}

	if fromPiece.IsNull() || (fromPiece.IsB() && toPiece.IsB()) || (fromPiece.IsW() && toPiece.IsW()) {
		return false
	}
	dy := to.Y - from.Y
	dx := to.X - from.X
	var bCrossRiver bool
	if fromPiece.IsB() {
		bCrossRiver = to.Y > 4
	} else {
		bCrossRiver = to.Y <= 4
	}
	switch fromPiece.ToLower() {
	case 'r': // 车
		if from.Y == to.Y || from.X == to.X {
			if rangeHasPiece(this, from, to) {
				return false
			}
		} else {
			return false
		}
	case 'n': // 马
		// 2个自然数相乘, 等于2只有 [1x2、2x1]才符合要求
		if abs(dy)*abs(dx) != 2 {
			return false
		}
		// 计算蹩脚点坐标
		var mustEmptyPoint PiecePoint
		if abs(dy) == 2 { // 纵向移动
			mustEmptyPoint.X = from.X
			mustEmptyPoint.Y = (from.Y + to.Y) / 2
		} else { // 横向移动
			mustEmptyPoint.X = (from.X + to.X) / 2
			mustEmptyPoint.Y = from.Y
		}
		if !this.getPiece(mustEmptyPoint).IsNull() {
			return false
		}
	case 'b': // 象
		if bCrossRiver { // 过河了
			return false
		}
		if abs(dy) != 2 || abs(dx) != 2 {
			return false
		}
		from.Y += dy / 2
		from.X += dx / 2
		piece := this.getPiece(from)
		if !piece.IsNull() {
			return false
		}
	case 'a': // 士
		if !fitRangeJiangShi(to) {
			return false
		}
		if abs(dy*dx) != 1 {
			return false
		}
	case 'k': // 将
		if !fitRangeJiangShi(to) {
			return false
		}
		// "将" 有一个特殊走法
		//   在同一列上, 起点和终点都是 "将", 并且起点终点之间没有多于的棋子, 起点的将能够直接到达终点的将
		if dx == 0 {
			if toPiece.ToLower() == 'j' && !rangeHasPiece(this, from, to) {
				return true
			}
		}
		if (abs(dy) == 1 && abs(dx) == 0) || (abs(dy) == 0 && abs(dx) == 1) {
			return true
		}
		return false
	case 'c': // 炮
		if dy != 0 && dx != 0 {
			return false
		}
		dL := getD(from.Y, to.Y)
		dC := getD(from.X, to.X)
		midHasPieceCount := 0
		for {
			from.Y += dL
			from.X += dC

			if from.Y == to.Y && from.X == to.X {
				break
			}
			if !this.getPiece(from).IsNull() {
				midHasPieceCount++
			}
		}
		if midHasPieceCount == 0 && toPiece.IsNull() {
			return true
		}
		if midHasPieceCount == 1 && !toPiece.IsNull() {
			return true
		}
		return false
	case 'p': // 兵
		// 左右移动, 如果已经过河-> 可以, 否则 -> 不行
		if abs(dx) == 1 && dy == 0 {
			if !bCrossRiver {
				return false
			}
			return true
		} else if abs(dy) == 1 && dx == 0 { // 上下移动, 不准后退
			var goBack bool
			if fromPiece.IsB() {
				goBack = dy < 0
			} else {
				goBack = dy > 0
			}
			if goBack {
				return false
			}
			return true
		} else {
			return false
		}

	default:
		return false
	}

	return true
}

func getD(fromP, toP int32) int32 {
	if fromP > toP {
		return -1
	}
	if fromP == toP {
		return 0
	}
	return 1
}

func rangeHasPiece(this *GameRoom, from, to PiecePoint) bool {
	dL := getD(from.Y, to.Y)
	dC := getD(from.X, to.X)

	for {
		from.Y += dL
		from.X += dC

		if from.Y == to.Y && from.X == to.X {
			break
		}
		if !this.getPiece(from).IsNull() {
			return true
		}
	}
	return false
}

func fitRangeJiangShi(p PiecePoint) bool {
	return p.X >= 3 && p.X <= 5 && (p.Y <= 2 || p.Y >= 7)
}

func abs(a int32) int32 {
	if a < 0 {
		a = -a
	}
	return a
}

func (this *GamePiece) ToLower() rune {
	return unicode.ToLower(rune(*this))
}

func (this *GameRoom) getPiece(p PiecePoint) GamePiece {
	return this.PanelFull[p.Y][p.X]
}

func (this GamePiece) IsW() bool {
	return unicode.IsUpper(rune(this))
}

func (this GamePiece) IsB() bool {
	return unicode.IsLower(rune(this))
}

func (this GamePiece) IsNull() bool {
	return !this.IsW() && !this.IsB()
}

type GameOverNotice struct {
	IsPeace     bool
	WinUsername string
}

func (this *GameRoom) DoMove(from PiecePoint, to PiecePoint) {
	toPiece := this.getPiece(to)
	this.PanelFull[to.Y][to.X] = this.PanelFull[from.Y][from.X]
	this.PanelFull[from.Y][from.X] = '0'

	winUserId := ``
	if toPiece.ToLower() == 'k' {
		winUserId = this.NextTurnUserID
	}
	oldNextTurnUserId := this.NextTurnUserID
	if this.NextTurnUserID == this.Data.BUserID {
		this.NextTurnUserID = this.Data.WUserID
	} else {
		this.NextTurnUserID = this.Data.BUserID
	}
	sessionList := getSessionListBy(dsession.UserID(this.NextTurnUserID))
	if len(sessionList) == 0 {
		log.Println("nextUser is nil")
		return
	}
	nextIsW := this.NextTurnUserID == this.Data.WUserID
	cannotMovePiece := true
	for y := int32(0); y < MAX_VALUE_Y && cannotMovePiece && winUserId == ``; y++ {
		for x := int32(0); x < MAX_VALUE_X && cannotMovePiece; x++ {
			thisPoint := PiecePoint{
				Y: y,
				X: x,
			}
			thisPiece := this.getPiece(thisPoint)
			if (nextIsW && thisPiece.IsW()) || (!nextIsW && thisPiece.IsB()) {
				resp := this.RpcGetSuggestion(sessionList[0], GetSuggestionRequest{
					FromPoint: thisPoint,
				})
				if len(resp.CanMoveToList) > 0 {
					cannotMovePiece = false
				}
			}
		}
	}
	if winUserId == `` && cannotMovePiece {
		winUserId = oldNextTurnUserId
	}
	this.sync2Client(nil)
	if winUserId != `` {
		this.onGameOver(winUserId)
	}
}

func (this *GameRoom) onGameOver(winUserId string) {
	this.Data.IsGameRunning = false
	this.sync2Client(nil)
	this.BroadcastToAll(GameOverNotice{
		IsPeace:     winUserId == ``,
		WinUsername: getUserNameByIdIgnoreEmpty(winUserId),
	})
}

func (this *GameRoom) formatShowStatus(resp *SyncPanelMessage, userId string) {
	resp.ShowReGame = false
	resp.ShowSiteDown = false
	if this.Data.WUserID != `` && this.Data.BUserID != `` && !this.Data.IsGameRunning {
		if this.Data.BUserID == userId || this.Data.WUserID == userId {
			resp.ShowReGame = true
		}
	}
	if !this.Data.IsGameRunning {
		if this.Data.BUserID == `` && this.Data.WUserID != userId {
			resp.ShowSiteDown = true
		} else if this.Data.WUserID == `` && this.Data.BUserID != userId {
			resp.ShowSiteDown = true
		}
	}
}

func (this *GameRoom) formatPanelV2() string {
	w := &bytes.Buffer{}
	for y := 0; y < MAX_VALUE_Y; y++ {
		emptyCnt := 0
		for x := 0; x < MAX_VALUE_X; x++ {
			if this.PanelFull[y][x] == '0' {
				emptyCnt++
				continue
			}
			if emptyCnt > 0 {
				w.WriteByte(byte('0' + emptyCnt))
				emptyCnt = 0
			}
			w.WriteByte(byte(this.PanelFull[y][x]))
		}
		if emptyCnt > 0 {
			w.WriteByte(byte('0' + emptyCnt))
		}
		if y == MAX_VALUE_Y-1 {
			w.WriteByte(' ')
		} else {
			w.WriteString("/")
		}
	}
	//rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w
	if this.NextTurnUserID == this.Data.WUserID {
		w.WriteByte('w')
	} else {
		w.WriteByte('b')
	}
	str := w.String()
	return str
}
