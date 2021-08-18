# ChessGame
网络版中国象棋 是一个网络象棋桌, 目前功能有注册、登陆、下棋、聊天四大功能.

## 部分截图
![用户注册](screenshot/register.png)
![用户登陆](screenshot/login.png)
![游戏主界面](screenshot/playgame.png)

## v2版本特点
* 使用纯go语言重写了服务端逻辑，账号密码使用leveldb储存，不依赖外部数据库，可直接运行在windows、linux的电脑上
* 使用websocket+json进行通信, 相对于二进制协议调试简单，不用手动分包，前端使用nginx代理后不占用对外端口
* 自动从go代码生成协议的序列化、反序列化、c++的class定义，Encode、Decode不用手动操作了
* 客户端使用rpchub与服务端进行通信
  * 对外表现为同步形式的rpc.
  * 客户端可直接注册服务端通知回调函数, 配合c++11 的lamda表达式，处理代码几行搞定
* 支持多个房间

## v2.5版本改进
![v2.5版本结构图](screenshot/v2.5-layout.png)
* 服务端分离为 gateway、game两大模块
  * gateway 的作用是对客户端连接进行代理、数据分包、数据封包、死链接检测、把底层链接抽象为redis里的一条sessionId
  * game 承载游戏全部逻辑, 只与redis直接通信, 支持客户端无感知的情况下快速重启进程
* redis的作用
  * 消息队列: 使用redis的list结构传递客户端连接信息、客户端读写数据信息
  * game进程重启时的内存状态暂存区
  * gateway进程的目前连接的session信息镜像，可直接用redis客户端看连接上来的客户端个数, sessionId信息
* game进程重启逻辑
  * 老的game进程收到kill信息后，停止消费redis消息队列里的消息，保存进程内的全部状态到redis，os.Exit(0)
  * 新的game进程启动时，先从redis加载上一个进程的状态到内存, 然后开始处理redis消息
  * 重启停机时间一般在100ms以内
## v2.6版本改进
 * 引入mysql做数据储存引擎，不再使用game进程的内存保存数据。
 * 使用[ent](https://github.com/ent/ent)做内存实体映射数据库
 * 改进上个版本的明文储存密码的缺陷，使用[bcrypt](https://golang.org/x/crypto/bcrypt)对用户密码进行散列
 * 使用[FEN文件格式](https://www.xqbase.com/protocol/cchess_fen.htm)传递棋盘信息，规范化棋子名称
 * 合并了 gateway、game两个二进制，但是还是需要启动2个进程，使用[cobra](https://github.com/spf13/cobra) 处理命令行逻辑
## ChessClient 需要
* qt  >= 5.6
## 引用项目
* 服务端:
    * [websocket](https://github.com/gorilla/websocket)
    * [ent](https://github.com/ent/ent)
    * [bcrypt](https://golang.org/x/crypto/bcrypt)
    * [cobra](https://github.com/spf13/cobra)

## nginx代理websocket注意事项
* 真实的ChessServer监听在127.0.0.1:8912位置, nginx配置为
````
server {
    listen       80;
    server_name  example.com;
    location / {
        proxy_pass   http://127.0.0.1:8912;
        proxy_redirect    off;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header Host $host;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;

        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
    }
}
````
## 使用docker-compose一键部署服务端
1. 克隆代码到本地
  `git clone https://github.com/orestonce/ChessGame`
2. 启动docker-compose
  `docker-compose up -d`
## 编译、部署服务端
1. 克隆本仓库代码到本地
  `git clone https://github.com/orestonce/ChessGame`
2. 设置 `GOPATH` 为克隆下来的的目录， 开始编译linux_amd64版服务端程序
  `GOPATH=$(pwd) go run src/BuildServer/main.go`
3. 编译完成，会产生1个二进制文件 bin/ChessServer
4. 部署到服务器: 假设redis地址为127.0.0.1:6379, 服务器地址为ws://192.168.56.111:8912/ChessGame, 部署前缀为chess001, 数据库为 "root:@tcp(mysql_server:3306)/chess?parseTime=true&charset=utf8"
  ````
  # 启动gateway
  ./ChessServer Gate --laddr 192.168.56.111:8912 --raddr 127.0.0.1:6379 -rprefix chess001
  # 启动game
  ./ChessServer Game --raddr 127.0.0.1:6379 -rprefix chess001 --mysql_schema "root:@tcp(mysql_server:3306)/chess?parseTime=true&charset=utf8"
  ````
![v2.5服务端1](screenshot/v2.5-server1.png)
## 编译、配置客户端
1. 克隆本仓库代码到本地
  `git clone https://github.com/orestonce/ChessGame`
2. 打开 `ChessClient/ChessClient.pro` ，开始编译
  `ctrl + r`
3. 在登陆页面点击 "设置服务器" 按钮, 输入服务器连接地址: `ws://192.168.56.111:8912/ChessGame`, 点击确认
4. 回到登陆页面后，点击注册，输入用户名、密码点击确认即可注册成功
5. 回到登陆页面后，输入用户名、密码点击登陆即可登入刚才在第4步创建的账号
6. 再开启一个客户端进程，重复4, 5步，登陆另一个账号即可和自己玩耍了

## 预编译的客户端、服务端
   * [Go](https://github.com/orestonce/ChessGame/releases)
