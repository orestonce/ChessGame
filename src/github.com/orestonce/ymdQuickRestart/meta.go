package ymdQuickRestart

type RedisInfo struct {
	RedisAddr string
	Prefix    string
}

type GatewayConnInfo struct {
	SessionId   string `json:",omitempty"`
	ConnectTime string `json:",omitempty"`
	ClientIp    string `json:",omitempty"`
}

type RedisExchange struct {
	SessionId string `json:",omitempty"`
	MsgType   string `json:",omitempty"`
	MsgData   []byte `json:",omitempty"`
}

const (
	MTLogicKick    = `MTLogicKick`    // GatewayIn: logic踢session
	MTLogicWrite   = `MTLogicWrite`   // GatewayIn: logic向session写数据
	MTGatewayNew   = `MTGatewayNew`   // LogicIn:   gateway接收到了一条新链接
	MTGatewayRead  = `MTGatewayRead`  // LogicIn:   gateway从已有的链接上读取到了一条消息
	MTGatewayClose = `MTGatewayClose` // LogicIn:   gateway关闭了一套链接
	MTTimeout      = `MTTimeout`      // 无: 每秒钟LogicService.GetMessageChan会被自动放一条该消息, 方便处理逻辑
)

// hash, gateway上的所有链接的 [sessionId, 创建时间]
func (this *RedisInfo) GatewaySession() string {
	return this.Prefix + "-gateway-session"
}

// list, 发给gateway的消息列表
func (this *RedisInfo) GatewayIn() string {
	return this.Prefix + "-gateway-in"
}

// list, 发给logic的消息列表
func (this *RedisInfo) LogicIn() string {
	return this.Prefix + "-logic-in"
}

// kv, logic的内存状态，logic启动的时候加载, 退出的时候保存
func (this *RedisInfo) MemoryState() string {
	return this.Prefix + "-memory"
}
