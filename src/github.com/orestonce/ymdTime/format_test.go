package ymdTime

import (
	"testing"
	"time"
	"github.com/orestonce/ymdAssert"
)

func TestDefaultFormat(t *testing.T) {
	tm := time.Date(2019, 3, 5, 16, 17, 18, 0, GetDefaultLocation())
	f1 := tm.Format(FormatDateTime)
	f2 := DefaultFormat(tm)
	ymdAssert.True(f1 == f2, f1, f2)
	ymdAssert.True(f1 == `2019-03-05 16:17:18`, f1)
}
