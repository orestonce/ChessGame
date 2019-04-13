package ymdJson

import "encoding/json"

func MustUnmarshal(data []byte, a interface{}) {
	err := json.Unmarshal(data, &a)
	if err != nil {
		panic(err)
	}
}

func MustMarshal(a interface{}) (data []byte) {
	data, err := json.Marshal(a)
	if err != nil {
		panic(err)
	}
	return
}
