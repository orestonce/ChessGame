package ymdError

import (
	"testing"
	"github.com/orestonce/ymdAssert"
	"strings"
)

func TestPanicToErrMsgAndStack(t *testing.T) {
	const randMessage = `y4m7r9fb`
	errMsg, stack := PanicToErrMsgAndStack(func() {
		panic(randMessage)
	})
	ymdAssert.True(errMsg == randMessage, errMsg)
	ymdAssert.True(strings.Contains(stack.String(), `error_test.go:12`))

	errMsg, _ = PanicToErrMsgAndStack(func() {})
	ymdAssert.True(errMsg == ``, errMsg)

	errMsg, stack = PanicToErrMsgAndStack(func() {
		panic(``)
	})
	ymdAssert.True(errMsg == unknownErrorMsg, errMsg)
	ymdAssert.True(strings.Contains(stack.String(), `error_test.go:21`), stack.String())
}
