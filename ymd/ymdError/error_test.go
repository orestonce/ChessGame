package ymdError

import (
	"fmt"
	"testing"
)

func TestPanicToErrMsgAndStack(t *testing.T) {
	const randMessage = `y4m7r9fb`
	errMsg, stack := PanicToErrMsgAndStack(func() {
		panic(randMessage)
	})
	fmt.Println(errMsg, stack)
}
