package main

import (
	"os/exec"
	"os"
	"github.com/orestonce/ymdError"
)

func main() {
	var err error
	err = os.Setenv(`GOOS`, `linux`)
	ymdError.PanicIfError(err)
	err = os.Setenv(`GOARCH`, `amd64`)
	ymdError.PanicIfError(err)
	err = os.MkdirAll(`bin`, 0755)
	ymdError.PanicIfError(err)
	err = os.Chdir(`bin`)
	ymdError.PanicIfError(err)
	err = exec.Command(`go`, `build`, `ChessServer/ChessGame`).Run()
	ymdError.PanicIfError(err)
	err = exec.Command(`go`, `build`, `ChessServer/ChessGate`).Run()
	ymdError.PanicIfError(err)
}
