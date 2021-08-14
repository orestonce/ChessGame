package main

import (
	"io/ioutil"
	"os"
	"path/filepath"
	"strings"
)

func main() {
	root := "D:\\go-projects\\ChessGame\\ChessClient\\images"
	list, err := ioutil.ReadDir(root)
	if err != nil {
		panic(err)
	}
	for _, one := range list {
		origin := filepath.Join(root, one.Name())
		after := filepath.Join(root, strings.ReplaceAll(strings.ReplaceAll(one.Name(), "u-", "w"), "d-", "b"))
		err = os.Rename(origin, after)
		if err != nil {
			panic(err)
		}
	}
}
