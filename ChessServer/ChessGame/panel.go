package ChessGame

import (
	"bytes"
	"unicode"
)

const (
	LINE_COUNT   = 10
	COLUMN_COUNT = 9
)

type GamePanel struct {
	room             *GameRoom `json:"-"`
	PanelFull        [LINE_COUNT][COLUMN_COUNT]GamePiece
	UpperUsername    string
	DownUsername     string
	IsGameRunning    bool
	NextTurnUsername string
}

type GamePiece byte

func (this *GamePanel) Reset(room *GameRoom) {
	this.room = room
	this.PanelFull = [LINE_COUNT][COLUMN_COUNT]GamePiece{
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
	}
}

func (this *GamePanel) LoadNormalPanelFull() {
	this.PanelFull = [LINE_COUNT][COLUMN_COUNT]GamePiece{
		{'C', 'M', 'X', 'S', 'J', 'S', 'X', 'M', 'C'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', 'P', '0', '0', '0', '0', '0', 'P', '0'},
		{'B', '0', 'B', '0', 'B', '0', 'B', '0', 'B'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'b', '0', 'b', '0', 'b', '0', 'b', '0', 'b'},
		{'0', 'p', '0', '0', '0', '0', '0', 'p', '0'},
		{'0', '0', '0', '0', '0', '0', '0', '0', '0'},
		{'c', 'm', 'x', 's', 'j', 's', 'x', 'm', 'c'},
	}
}

func (this *GamePanel) onUserLeave(username string) {
	winUsername := ``
	if this.DownUsername == username {
		this.DownUsername = ``
		winUsername = this.UpperUsername
	} else if this.UpperUsername == username {
		this.UpperUsername = ``
		winUsername = this.DownUsername
	} else {
		return
	}
	if this.IsGameRunning {
		this.onGameOver(winUsername)
	} else {
		this.room.sync2Client(nil)
	}
}

func (this *GamePanel) CanMove(who string, from PiecePoint, to PiecePoint) bool {
	fromPiece := this.getPiece(from)
	toPiece := this.getPiece(to)

	if !from.IsValid() || !to.IsValid() {
		return false
	}
	if this.NextTurnUsername != who {
		return false
	}

	if fromPiece.IsUpper() && this.NextTurnUsername != this.UpperUsername ||
		fromPiece.IsDown() && this.NextTurnUsername != this.DownUsername {
		return false
	}

	if fromPiece.IsNull() || (fromPiece.IsDown() && toPiece.IsDown()) || (fromPiece.IsUpper() && toPiece.IsUpper()) {
		return false
	}
	dLine := to.Line - from.Line
	dColumn := to.Column - from.Column
	var bCrossRiver bool
	if fromPiece.IsUpper() {
		bCrossRiver = (to.Line > 4)
	} else {
		bCrossRiver = (to.Line < 4)
	}

	switch fromPiece.ToLower() {
	case 'c': // 车
		if from.Line == to.Line || from.Column == to.Column {
			if rangeHasPiece(this, from, to) {
				return false
			}
		} else {
			return false
		}
	case 'm':
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
	case 'x':
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
	case 's':
		if !fitRangeJiangShi(to) {
			return false
		}
		if abs(dLine*dColumn) != 1 {
			return false
		}
	case 'j':
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
	case 'p':
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
				midHasPieceCount ++
			}
		}
		if midHasPieceCount == 0 && toPiece.IsNull() {
			return true
		}
		if midHasPieceCount == 1 && !toPiece.IsNull() {
			return true
		}
		return false
	case 'b':
		// 左右移动, 如果已经过河-> 可以, 否则 -> 不行
		if abs(dColumn) == 1 && dLine == 0 {
			if !bCrossRiver {
				return false
			}
			return true
		} else if (abs(dLine) == 1 && dColumn == 0) { // 上下移动, 不准后退
			var goback bool
			if fromPiece.IsUpper() {
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

func rangeHasPiece(this *GamePanel, from, to PiecePoint) bool {
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

func (this *GamePanel) getPiece(p PiecePoint) GamePiece {
	return this.PanelFull[p.Line][p.Column]
}

func (this *GamePiece) IsUpper() bool {
	return unicode.IsUpper(rune(*this))
}

func (this *GamePiece) IsDown() bool {
	return unicode.IsLower(rune(*this))
}

func (this GamePiece) IsNull() bool {
	return !this.IsUpper() && !this.IsDown()
}

type GameOverNotice struct {
	IsPeace     bool
	WinUsername string
}

func (this *GamePanel) DoMove(from PiecePoint, to PiecePoint) {
	toPiece := this.getPiece(to)
	this.PanelFull[to.Line][to.Column] = this.PanelFull[from.Line][from.Column]
	this.PanelFull[from.Line][from.Column] = '0'

	winUsername := ``
	if toPiece.ToLower() == 'j' {
		winUsername = this.NextTurnUsername
	}
	oldNextTurn := this.NextTurnUsername
	if this.NextTurnUsername == this.DownUsername {
		this.NextTurnUsername = this.UpperUsername
	} else {
		this.NextTurnUsername = this.DownUsername
	}
	nextIsUpper := this.NextTurnUsername == this.UpperUsername
	cannotMovePiece := true
	for line := int32(0); line < LINE_COUNT && cannotMovePiece && winUsername == ``; line ++ {
		for column := int32(0); column < COLUMN_COUNT && cannotMovePiece; column++ {
			thisPoint := PiecePoint{
				Line:   line,
				Column: column,
			}
			thisPiece := this.getPiece(thisPoint)
			if (nextIsUpper && thisPiece.IsUpper()) || (!nextIsUpper && thisPiece.IsDown()) {
				resp := this.room.RpcGetSuggestion(this.NextTurnUsername, GetSuggestionRequest{
					FromPoint: thisPoint,
				})
				if len(resp.CanMoveToList) > 0 {
					cannotMovePiece = false
				}
			}
		}
	}
	if winUsername == `` && cannotMovePiece {
		winUsername = oldNextTurn
	}
	this.room.sync2Client(nil)
	if winUsername != `` {
		this.onGameOver(winUsername)
	}
}

func (this *GamePanel) onGameOver(winUsername string) {
	this.IsGameRunning = false
	this.room.sync2Client(nil)
	pkt := GameOverNotice{
		IsPeace:     winUsername == ``,
		WinUsername: winUsername,
	}
	this.room.BroadcastToAll(pkt)
}

func (this *GamePanel) formatShowStatus(resp *SyncPanelMessage, username string) {
	resp.ShowReGame = false
	resp.ShowSiteDown = false
	if this.UpperUsername != `` && this.DownUsername != `` && !this.IsGameRunning {
		if this.DownUsername == username || this.UpperUsername == username {
			resp.ShowReGame = true;
		}
	}
	if !this.IsGameRunning {
		if this.DownUsername == `` && this.UpperUsername != username {
			resp.ShowSiteDown = true
		} else if this.UpperUsername == `` && this.DownUsername != username {
			resp.ShowSiteDown = true
		}
	}
}

func (this *GamePanel) formatPanelFull(resp *SyncPanelMessage) {
	w := &bytes.Buffer{}
	for line := 0; line < LINE_COUNT; line++ {
		for column := 0; column < COLUMN_COUNT; column++ {
			w.WriteByte(byte(this.PanelFull[line][column]))
		}
	}
	resp.PanelFull = w.String()
	resp.UpperUsername = this.UpperUsername
	resp.DownUsername = this.DownUsername
	resp.IsGameRunning = this.IsGameRunning
	resp.NextTurnUsername = this.NextTurnUsername
}
