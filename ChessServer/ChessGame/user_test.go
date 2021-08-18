package ChessGame

import (
	"github.com/orestonce/ChessGame/ymd/ymdAssert"
	"testing"
)

func TestParseFenMove(t *testing.T) {
	from, to, ok := ParseFenMove(`c3c4`)
	ymdAssert.True(ok)
	ymdAssert.True(from.Y == 6 && from.X == 2, from)
	ymdAssert.True(to.Y == 5 && to.X == 2, to)
}
