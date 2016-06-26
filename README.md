# ChessGame
网络版中国象棋 是一个网络象棋桌, 目前功能有注册、登陆、下棋、聊天四大功能.

## 部分截图
![用户注册](https://github.com/orestonce/ChessGame/raw/master/screenshot/register.png)
![用户登陆](https://github.com/orestonce/ChessGame/raw/master/screenshot/login.png)
![游戏主界面](https://github.com/orestonce/ChessGame/raw/master/screenshot/playgame.png)

## ChessClient 依赖
* gcc >= 4.8
* qt  >= 4.8
* c++11 序列化工具 [ cereal ] ( https://github.com/USCiLab/cereal )

## ChessServer 依赖
* gcc >= 4.8
* c++11 序列化工具 [ cereal ] ( https://github.com/USCiLab/cereal )
* mysql开发包 [ mysql-devel ]
* libevent 开发包 [ libevent-devel ] ( http://libevent.org )

## 引用项目
* [ cereal c++11 序列化库 ] ( https://github.com/USCiLab/cereal ) 
* [ mysqlquerypp mysql简单的c++包装 ] ( https://github.com/flurreh/mysqlquerypp )
* [ libevent 高性能、跨平台的事件通知网络库 ] ( http://libevent.org/ )
* [ inifile2 ini文件读写库 ] ( https://github.com/Winnerhust/inifile2 )

## 部署ChessServer
1. 将 chess.sql 倒入到数据库
2. 编辑chess_config.ini 文件, 设置对应mysql数据库字段和监听端口等
3. 将chess_config.ini文件放到编译出来的 ChessServer 同级目录下, 启动ChessServer: ./ChessServer

## 部署 ChessClient
* 这个没什么好部署的, 只需要将对应的ChessServer地址设置好然后注册登陆即可

## 欢迎报告BUG和提交功能需求

### windows 客户端直接下载
* [ ChessClient.rar ] ( http://pan.baidu.com/s/1nv3v8bJ )
