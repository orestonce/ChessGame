package ymdRedis

import (
	"github.com/orestonce/ChessGame/ymd/ymdAssert"
	"testing"
)

const key = `key`
const filed = `filed`
const filed1 = `filed1`
const value = `value`
const value1 = `value1`

// kv
func TestNewRedisClient(t *testing.T) {
	client := getTestClient()
	client.MustFlushAll()
	ymdAssert.True(`` == client.MustGetIgnoreNotExists(key))
	client.MustSet(key, value)
	ymdAssert.True(client.MustGetIgnoreNotExists(key) == value)
}

// list
func TestRedisClient_MustBLPop(t *testing.T) {
	client := getTestClient()
	client.MustFlushAll()
	client.MustRPush(key, value)
	client.MustRPush(key, value1)
	result, ok := client.MustBLPop(1, key)
	ymdAssert.True(ok && result == value)
	result, ok = client.MustBLPop(1, key)
	ymdAssert.True(ok && result == value1)
	_, ok = client.MustBLPop(1, key)
	ymdAssert.True(!ok)
}

// hash
func TestRedisClient_MustHGetAll(t *testing.T) {
	client := getTestClient()
	client.MustFlushAll()
	ok := client.MustHSetNx(key, filed, value)
	ymdAssert.True(ok)
	ok = client.MustHSetNx(key, filed, value1)
	ymdAssert.True(!ok)
	ok = client.MustHSetNx(key, filed1, value1)
	ymdAssert.True(ok)
	filedValueMap := client.MustHGetAll(key)
	ymdAssert.True(len(filedValueMap) == 2)
	ymdAssert.True(filedValueMap[filed] == value)
	ymdAssert.True(filedValueMap[filed1] == value1)

	ok = client.MustHDel(key, filed)
	ymdAssert.True(ok)
	ok = client.MustHDel(key, filed)
	ymdAssert.True(!ok)

	filedValueMap = client.MustHGetAll(key)
	ymdAssert.True(len(filedValueMap) == 1)
	ymdAssert.True(filedValueMap[filed1] == value1)
}

func getTestClient() *RedisClient {
	return NewRedisClient(`192.168.56.111:6379`)
}
