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
	LINE_COUNT   = 10
	COLUMN_COUNT = 9
)

type GamePiece byte

func (this *GameRoom) LoadPanelFromData() {
	if this.Data == nil || this.Data.Panel == "" {
		this.PanelFull = [LINE_COUNT][COLUMN_COUNT]GamePiece{
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
		for line := 0; line < LINE_COUNT; line++ {
			r := strings.NewReader(lineData[line])
			for col := 0; col < COLUMN_COUNT; {
				ch, err := r.ReadByte()
				if err != nil {
					log.Println("Error panel data", this.Data.Panel, lineData[line], err)
					return
				}
				if '0' < ch && ch <= '9' {
					emptyCnt := int(ch - '0')
					for emptyCnt > 0 && col < COLUMN_COUNT {
						this.PanelFull[line][col] = '0'
						col++
						emptyCnt--
					}
				} else {
					this.PanelFull[line][col] = GamePiece(ch)
					col++
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
	dLine := to.Y - from.Y
	dColumn := to.X - from.X
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
		if abs(dLine)*abs(dColumn) != 2 {
			return false
		}
		// 计算蹩脚点坐标
		var mustEmptyPoint PiecePoint
		if abs(dLine) == 2 { // 纵向移动
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
		if abs(dLine) != 2 || abs(dColumn) != 2 {
			return false
		}
		from.Y += dLine / 2
		from.X += dColumn / 2
		piece := this.getPiece(from)
		if !piece.IsNull() {
			return false
		}
	case 'a': // 士
		if !fitRangeJiangShi(to) {
			return false
		}
		if abs(dLine*dColumn) != 1 {
			return false
		}
	case 'k': // 将
		if !fitRangeJiangShi(to) {
			return false
		}
		// "将" 有一个特殊走法
		//   在同一列上, 起点和终点都是 "将", 并且起点终点之间没有多于的棋子, 起点的将能够直接到达终点的将
		if dColumn == 0 {
			if toPiece.ToLower() == 'j' && !rangeHasPiece(this, from, to) {
				return true
			}
		}
		if (abs(dLine) == 1 && abs(dColumn) == 0) || (abs(dLine) == 0 && abs(dColumn) == 1) {
			return true
		}
		return false
	case 'c': // 炮
		if dLine != 0 && dColumn != 0 {
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
		if abs(dColumn) == 1 && dLine == 0 {
			if !bCrossRiver {
				return false
			}
			return true
		} else if abs(dLine) == 1 && dColumn == 0 { // 上下移动, 不准后退
			var goBack bool
			if fromPiece.IsB() {
				goBack = dLine < 0
			} else {
				goBack = dLine > 0
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
	for line := int32(0); line < LINE_COUNT && cannotMovePiece && winUserId == ``; line++ {
		for column := int32(0); column < COLUMN_COUNT && cannotMovePiece; column++ {
			thisPoint := PiecePoint{
				Y: line,
				X: column,
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
	for line := 0; line < LINE_COUNT; line++ {
		emptyCnt := 0
		for column := 0; column < COLUMN_COUNT; column++ {
			if this.PanelFull[line][column] == '0' {
				emptyCnt++
				continue
			}
			if emptyCnt > 0 {
				w.WriteByte(byte('0' + emptyCnt))
				emptyCnt = 0
			}
			w.WriteByte(byte(this.PanelFull[line][column]))
		}
		if emptyCnt > 0 {
			w.WriteByte(byte('0' + emptyCnt))
		}
		if line == LINE_COUNT-1 {
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
