package ymdError

import "fmt"

func PanicToErrorAndStack(f func()) (err error, stack Stack) {
	defer func() {
		ei := recover()
		if ei == nil {
			return
		}
		switch out := ei.(type) {
		case error:
			err = out
		default:
			err = fmt.Errorf("%v", out)
		}
		stack = getAllStack(4)
	}()
	f()
	return
}
