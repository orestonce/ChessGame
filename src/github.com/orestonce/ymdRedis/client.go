package ymdRedis

import (
	"github.com/go-redis/redis"
	"github.com/orestonce/ymdError"
	"time"
)

type RedisClient struct {
	client *redis.Client
}

func NewRedisClient(addr string) *RedisClient {
	client := redis.NewClient(&redis.Options{
		Addr: addr,
	})
	return &RedisClient{client: client,}
}

//func (this *RedisClient) MustPing() {
//	_, err := this.client.Ping().Result()
//	ymdError.PanicIfError(err)
//}

func (this *RedisClient) MustSet(k string, v string) {
	_, err := this.client.Set(k, v, 0).Result()
	ymdError.PanicIfError(err)
}

func (this *RedisClient) MustFlushAll() {
	_, err := this.client.FlushAll().Result()
	ymdError.PanicIfError(err)
}

func (this *RedisClient) MustHSetNx(key string, filed string, value string) (ok bool) {
	ok, err := this.client.HSetNX(key, filed, value).Result()
	ymdError.PanicIfError(err)
	return ok
}

func (this *RedisClient) MustHDel(key string, filed string) (ok bool) {
	s, err := this.client.HDel(key, filed).Result()
	ymdError.PanicIfError(err)
	return s == 1
}

func (this *RedisClient) MustHGetAll(key string) (filedValueMap map[string]string) {
	filedValueMap, err := this.client.HGetAll(key).Result()
	ymdError.PanicIfError(err)
	return
}

//func (this *RedisClient) MustDelete(key string) (ok bool) {
//	status := this.client.Del(key)
//	i64, err := status.Result()
//	ymdError.PanicIfError(err)
//	return i64 == 1
//}

//func (this *RedisClient) MustSetNx(k string, v string) (ok bool) {
//	ok, err := this.client.SetNX(k, v, 0).Result()
//	ymdError.PanicIfError(err)
//	return
//}

//func (this *RedisClient) MustCompareAndDelete(key string, old string) bool {
//	cmd := this.client.Eval(`
//local key=ARGV[1];
//local old=ARGV[2];
//if redis.call('get', key)==old then
//       redis.call('del', key)
//       return 1;
//end;
//return 0;
//`, nil, []string{
//		key,
//		old,
//	})
//	i64, err := cmd.Int64()
//	ymdError.PanicIfError(err)
//	return i64 == 1
//}

func (this *RedisClient) MustGetIgnoreNotExists(k string) (v string) {
	vs, err := this.client.Get(k).Bytes()
	if err == redis.Nil {
		return ``
	}
	ymdError.PanicIfError(err)
	return string(vs)
}

//func (this *RedisClient) MustIsExists(key string) bool {
//	i64, err := this.client.Exists(key).Result()
//	ymdError.PanicIfError(err)
//	return i64 == 1
//}

func (this *RedisClient) MustRPush(key string, value string) {
	_, err := this.client.RPush(key, value).Result()
	ymdError.PanicIfError(err)
}

func (this *RedisClient) MustBLPop(seconds int, key string) (value string, hasData bool) {
	tmp, err := this.client.BLPop(time.Second*time.Duration(seconds), key).Result()
	if err == redis.Nil {
		return ``, false
	}
	ymdError.PanicIfError(err)
	return tmp[1], true
}
