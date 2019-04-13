package ymdSync

import "sync"

type StringMutexMap struct {
	locker sync.Mutex
	data   map[string]*sync.Mutex
}

func (this *StringMutexMap) LockByString(s string) {
	this.locker.Lock()
	defer this.locker.Unlock()

	if this.data == nil {
		this.data = map[string]*sync.Mutex{}
	}
	mtx := &sync.Mutex{}
	this.data[s] = mtx
	mtx.Lock()
}

func (this *StringMutexMap) UnlockByString(s string) {
	this.locker.Lock()
	defer this.locker.Unlock()

	if this.data == nil {
		panic(`vd2e8wxj`)
	}
	item := this.data[s]
	if item == nil {
		panic(`curbrn9w`)
	}
	item.Unlock()
	delete(this.data, s)
}
