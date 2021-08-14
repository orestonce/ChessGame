package ChessGame

import (
	"bytes"
	"context"
	"github.com/orestonce/ChessGame/ent"
	"github.com/orestonce/ChessGame/ent/droom"
	"github.com/orestonce/ChessGame/ent/dsession"
	"log"
	"unicode"
)

const (
	LINE_COUNT   = 10
	COLUMN_COUNT = 9
)

type GamePiece byte

func (this *GameRoom) LoadPanelFromData() {
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

func (this *GameRoom) SaveRoomDataToDb() {
	data := this.Data
	if data == nil {
		return
	}
	var resp SyncPanelMessage
	this.formatPanelFull(&resp)
	data.Panel = resp.PanelFull
	_, err := gDbClient.DRoom.Update().Where(droom.ID(this.RoomId)).
		SetUpUserID(data.UpUserID).SetDownUserID(data.DownUserID).SetPanel(resp.PanelFull).SetIsGameRunning(data.IsGameRunning).Save(context.Background())
	if err != nil {
		log.Println("GameRoom.SaveRoomDataToDb", err)
	}
}

func (this *GameRoom) onUserLeave(userId string) {
	winUserId := ``
	if this.Data.DownUserID == userId {
		this.Data.DownUserID = ``
		winUserId = this.Data.UpUserID
	} else if this.Data.UpUserID == userId {
		this.Data.UpUserID = ``
		winUserId = this.Data.UpUserID
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
	if this.NextTurnUserId != session.UserID {
		return false
	}

	if fromPiece.IsUpper() && this.NextTurnUserId != this.Data.UpUserID ||
		fromPiece.IsDown() && this.NextTurnUserId != this.Data.DownUserID {
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
	case 'b':
		// 左右移动, 如果已经过河-> 可以, 否则 -> 不行
		if abs(dColumn) == 1 && dLine == 0 {
			if !bCrossRiver {
				return false
			}
			return true
		} else if abs(dLine) == 1 && dColumn == 0 { // 上下移动, 不准后退
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

func (this *GameRoom) DoMove(from PiecePoint, to PiecePoint) {
	toPiece := this.getPiece(to)
	this.PanelFull[to.Line][to.Column] = this.PanelFull[from.Line][from.Column]
	this.PanelFull[from.Line][from.Column] = '0'

	winUserId := ``
	if toPiece.ToLower() == 'j' {
		winUserId = this.NextTurnUserId
	}
	oldNextTurnUserId := this.NextTurnUserId
	if this.NextTurnUserId == this.Data.DownUserID {
		this.NextTurnUserId = this.Data.UpUserID
	} else {
		this.NextTurnUserId = this.Data.DownUserID
	}
	sessionList := getSessionListBy(dsession.UserID(this.NextTurnUserId))
	if len(sessionList) == 0 {
		log.Println("nextUser is nil")
		return
	}
	nextIsUpper := this.NextTurnUserId == this.Data.UpUserID
	cannotMovePiece := true
	for line := int32(0); line < LINE_COUNT && cannotMovePiece && winUserId == ``; line++ {
		for column := int32(0); column < COLUMN_COUNT && cannotMovePiece; column++ {
			thisPoint := PiecePoint{
				Line:   line,
				Column: column,
			}
			thisPiece := this.getPiece(thisPoint)
			if (nextIsUpper && thisPiece.IsUpper()) || (!nextIsUpper && thisPiece.IsDown()) {
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

func (this *GameRoom) formatShowStatus(resp *SyncPanelMessage, username string) {
	resp.ShowReGame = false
	resp.ShowSiteDown = false
	user := getUserByName(username)
	if user == nil {
		return
	}
	userId := user.ID

	if this.Data.UpUserID != `` && this.Data.DownUserID != `` && !this.Data.IsGameRunning {
		if this.Data.DownUserID == userId || this.Data.UpUserID == userId {
			resp.ShowReGame = true
		}
	}
	if !this.Data.IsGameRunning {
		if this.Data.DownUserID == `` && this.Data.DownUserID != username {
			resp.ShowSiteDown = true
		} else if this.Data.UpUserID == `` && this.Data.DownUserID != username {
			resp.ShowSiteDown = true
		}
	}
}

func (this *GameRoom) formatPanelFull(resp *SyncPanelMessage) {
	w := &bytes.Buffer{}
	for line := 0; line < LINE_COUNT; line++ {
		for column := 0; column < COLUMN_COUNT; column++ {
			w.WriteByte(byte(this.PanelFull[line][column]))
		}
	}
	resp.PanelFull = w.String()
	resp.UpperUsername = getUserNameByIdIgnoreEmpty(this.Data.UpUserID)
	resp.DownUsername = getUserNameByIdIgnoreEmpty(this.Data.DownUserID)
	resp.IsGameRunning = this.Data.IsGameRunning
	resp.NextTurnUsername = getUserNameByIdIgnoreEmpty(this.NextTurnUserId)
}
