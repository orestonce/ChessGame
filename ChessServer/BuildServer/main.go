package main

import (
	"github.com/orestonce/ChessGame/ymd/ymdError"
	"os"
	"os/exec"
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
	err = exec.Command(`go`, `build`, `github.com/orestonce/ChessGame/ChessServer`).Run()
	ymdError.PanicIfError(err)
}
