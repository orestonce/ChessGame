package ymdError

import (
	"runtime"
	"bytes"
	"fmt"
)

type stackFuncCall struct {
	FuncName string
	File     string
	Line     int
	Pc       uintptr
}

type Stack []stackFuncCall

// 读取调用栈信息
func getAllStack(skip int) Stack {
	pcs := make([]uintptr, 32)
	funcLen := runtime.Callers(skip, pcs)
	pcs = pcs[:funcLen-2]
	stacks := make([]stackFuncCall, 0)

	for _, pc := range pcs {
		f := runtime.FuncForPC(pc)
		s := stackFuncCall{}
		s.FuncName = f.Name()
		s.File, s.Line = f.FileLine(pc)
		s.Pc = pc
		stacks = append(stacks, s)
	}
	return Stack(stacks)
}

// 转换为string
func (this Stack) String() string {
	buf := bytes.NewBuffer(nil)
	for _, sc := range []stackFuncCall(this) {
		fmt.Fprintf(buf, "%v:%v:%v\n", sc.File, sc.Line, sc.FuncName)
	}
	return buf.String()
}
