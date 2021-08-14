package ymdAssert_test

import (
	"github.com/orestonce/ChessGame/ymd/ymdAssert"
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"testing"
)

func TestTrue(t *testing.T) {
	ymdAssert.True(true)
	const randMessage = `3un29a9n`
	errMsg, _ := ymdError.PanicToErrMsgAndStack(func() {
		ymdAssert.True(false, randMessage)
	})
	ymdAssert.True(errMsg == randMessage)
}
