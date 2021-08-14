package ChessGame

import (
	"bytes"
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
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
	}
}

func (this *GameRoom) SaveRoomDataToDb() {
	data := this.Data
	if data == nil {
		return
	}
	data.Panel = this.formatPanelV2()
	_, err := gDbClient.DRoom.Update().Where(droom.ID(this.RoomId)).
		SetWUserID(data.WUserID).SetBUserID(data.BUserID).SetPanel(data.Panel).SetIsGameRunning(data.IsGameRunning).
		SetNextTurnUserID(data.NextTurnUserID).Save(context.Background())
	if err != nil {
		log.Println("GameRoom.SaveRoomDataToDb", err)
	}
}

func (this *GameRoom) onUserLeave(userId string) {
	winUserId := ``
	if this.Data.BUserID == userId {
		this.Data.BUserID = ``
		winUserId = this.Data.WUserID
	} else if this.Data.WUserID == userId {
		this.Data.WUserID = ``
		winUserId = this.Data.WUserID
	} else {
		return
	}
	if this.Data.IsGameRunning {
		this.onGameOver(winUserId)
	} else {
		this.sync2Client(nil)
	}
}

func (this *GameRoom) CanMove(session *ent.DSession, from PiecePoint, to PiecePoint) bool {
	fromPiece := this.getPiece(from)
	toPiece := this.getPiece(to)

	if !from.IsValid() || !to.IsValid() {
		return false
	}
	if this.Data.NextTurnUserID != session.UserID {
		return false
	}

	if fromPiece.IsW() && this.Data.NextTurnUserID != this.Data.WUserID || fromPiece.IsB() && this.Data.NextTurnUserID != this.Data.BUserID {
		return false
	}

	if fromPiece.IsNull() || (fromPiece.IsB() && toPiece.IsB()) || (fromPiece.IsW() && toPiece.IsW()) {
		return false
	}
	dLine := to.Line - from.Line
	dColumn := to.Column - from.Column
	var bCrossRiver bool
	if fromPiece.IsW() {
		bCrossRiver = to.Line > 4
	} else {
		bCrossRiver = to.Line < 4
	}

	switch fromPiece.ToLower() {
	case 'r': // 车
		if from.Line == to.Line || from.Column == to.Column {
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
			mustEmptyPoint.Column = from.Column
			mustEmptyPoint.Line = (from.Line + to.Line) / 2
		} else { // 横向移动
			mustEmptyPoint.Column = (from.Column + to.Column) / 2
			mustEmptyPoint.Line = from.Line
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
		from.Line += dLine / 2
		from.Column += dColumn / 2
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
		dL := getD(from.Line, to.Line)
		dC := getD(from.Column, to.Column)
		midHasPieceCount := 0
		for {
			from.Line += dL
			from.Column += dC

			if from.Line == to.Line && from.Column == to.Column {
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
			var goback bool
			if fromPiece.IsW() {
				goback = dLine < 0
			} else {
				goback = dLine > 0
			}
			if goback {
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
	dL := getD(from.Line, to.Line)
	dC := getD(from.Column, to.Column)

	for {
		from.Line += dL
		from.Column += dC

		if from.Line == to.Line && from.Column == to.Column {
			break
		}
		if !this.getPiece(from).IsNull() {
			return true
		}
	}
	return false
}

func fitRangeJiangShi(p PiecePoint) bool {
	return p.Column >= 3 && p.Column <= 5 && (p.Line <= 2 || p.Line >= 7)
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
	return this.PanelFull[p.Line][p.Column]
}

func (this *GamePiece) IsW() bool {
	return unicode.IsLower(rune(*this))
}

func (this *GamePiece) IsB() bool {
	return unicode.IsUpper(rune(*this))
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
	this.PanelFull[to.Line][to.Column] = this.PanelFull[from.Line][from.Column]
	this.PanelFull[from.Line][from.Column] = '0'

	winUserId := ``
	if toPiece.ToLower() == 'k' {
		winUserId = this.Data.NextTurnUserID
	}
	oldNextTurnUserId := this.Data.NextTurnUserID
	if this.Data.NextTurnUserID == this.Data.BUserID {
		this.Data.NextTurnUserID = this.Data.WUserID
	} else {
		this.Data.NextTurnUserID = this.Data.BUserID
	}
	sessionList := getSessionListBy(dsession.UserID(this.Data.NextTurnUserID))
	if len(sessionList) == 0 {
		log.Println("nextUser is nil")
		return
	}
	nextIsUpper := this.Data.NextTurnUserID == this.Data.WUserID
	cannotMovePiece := true
	for line := int32(0); line < LINE_COUNT && cannotMovePiece && winUserId == ``; line++ {
		for column := int32(0); column < COLUMN_COUNT && cannotMovePiece; column++ {
			thisPoint := PiecePoint{
				Line:   line,
				Column: column,
			}
			thisPiece := this.getPiece(thisPoint)
			if (nextIsUpper && thisPiece.IsW()) || (!nextIsUpper && thisPiece.IsB()) {
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
	pkt := GameOverNotice{
		IsPeace:     winUserId == ``,
		WinUsername: getUserNameByIdIgnoreEmpty(winUserId),
	}
	this.BroadcastToAll(pkt)
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

func (this *GameRoom) formatPanelFull(resp *SyncPanelMessage) {
	resp.PanelFull = this.formatPanelV2()
	resp.WUserName = getUserNameByIdIgnoreEmpty(this.Data.WUserID)
	resp.WUserId = this.Data.WUserID
	resp.BUserName = getUserNameByIdIgnoreEmpty(this.Data.BUserID)
	resp.BUserId = this.Data.BUserID
	resp.IsGameRunning = this.Data.IsGameRunning
	resp.NextTurnUserId = this.Data.NextTurnUserID
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
	if this.Data.NextTurnUserID == this.Data.WUserID {
		w.WriteByte('w')
	} else {
		w.WriteByte('b')
	}
	str := w.String()
	//log.Println("format", str)
	return str
}
