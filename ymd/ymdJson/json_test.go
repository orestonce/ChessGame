package ymdJson

import (
	"github.com/orestonce/ChessGame/ymd/ymdAssert"
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"strings"
	"testing"
)

func TestMustMarshal(t *testing.T) {
	ymdAssert.True(`10` == MustMarshalToString(10))

	var tmp struct {
		Date int `json:"date"`
	}
	MustUnmarshalFromString(`{ "date" : 123}`,
		&tmp,
	)
	ymdAssert.True(tmp.Date == 123)
}

func TestMustUnmarshal(t *testing.T) {
	errMsg, stack := ymdError.PanicToErrMsgAndStack(func() {
		var data int
		MustUnmarshalFromString(``, &data)
	})
	ymdAssert.True(errMsg != ``)
	ymdAssert.True(strings.Contains(stack.String(), `json_test.go:26`), stack.String())
}
