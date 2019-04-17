package ymdAssert_test

import (
	"testing"
	"github.com/orestonce/ymdAssert"
	"github.com/orestonce/ymdError"
)

func TestTrue(t *testing.T) {
	ymdAssert.True(true)
	const randMessage = `3un29a9n`
	errMsg, _ := ymdError.PanicToErrMsgAndStack(func() {
		ymdAssert.True(false, randMessage)
	})
	ymdAssert.True(errMsg == randMessage)
}
