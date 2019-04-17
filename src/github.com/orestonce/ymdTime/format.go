package ymdTime

import (
	"time"
	"sync"
	"log"
)

const (
	FormatDateTime = `2006-01-02 15:04:05`
)

func DefaultFormat(t time.Time) string {
	return t.In(GetDefaultLocation()).Format(FormatDateTime)
}

var gDefaultLocationOnce sync.Once
var gDefaultLocation *time.Location

func GetDefaultLocation() *time.Location {
	gDefaultLocationOnce.Do(func() {
		local, err := time.LoadLocation("Asia/Chongqing")
		if err != nil {
			log.Fatal("ymdTime.GetDefaultLocation", err)
		}
		gDefaultLocation = local
	})
	return gDefaultLocation
}
