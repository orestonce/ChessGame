package ymdError

import "fmt"

const unknownErrorMsg = `83vdgt8r`

func PanicToErrMsgAndStack(f func()) (errMsg string, stack Stack) {
	defer func() {
		ei := recover()
		if ei == nil {
			return
		}
		errMsg = fmt.Sprint(ei)
		if errMsg == `` {
			errMsg = unknownErrorMsg
		}
		stack = getAllStack(4)
	}()
	f()
	return
}

func PanicIfError(err error) {
	if err != nil {
		panic(err)
	}
}
