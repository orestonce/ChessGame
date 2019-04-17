package ymdAssert

import "log"

func True(ok bool, a ... interface{}) {
	if !ok {
		log.Panic(a...)
	}
}
