package ChessCore

import (
	"encoding/json"
)

type CSExchangePacket struct {
	PacketType string
	Method     string
	Data       json.RawMessage
}

const (
	PtRpcCall   = `PtRpcCall`   // c->s
	PtRpcReply  = `PtRpcReply`  // s->c
	PtRpcNotice = `PtRpcNotice` // s->c
)

