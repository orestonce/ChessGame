#include "rpchub.h"
#include <QEventLoop>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

RpcHub::RpcHub()
{
    socket = 0;
    socketIsError = false;
}

bool RpcHub::InitWebsocket(const QString &wsUrl,int timeoutSecond)
{
    this->socket = new QWebSocket;
    this->socket->setParent(this);

    bool connectSuccess = false;
    {
        QEventLoop *connectLoop = new QEventLoop;
        QTimer *connectTimer = new QTimer;

        connect(this->socket, &QWebSocket::connected, [=, &connectSuccess](){
            connectSuccess = true;
            connectLoop->quit();
        });
        connect(connectTimer, &QTimer::timeout, [=](){
            connectLoop->quit();
            connectTimer->stop();
        });
        connectTimer->start(1000 * timeoutSecond);
        this->socket->open(QUrl(wsUrl));
        connectLoop->exec();
        connectTimer->stop();
        delete connectLoop;
        delete connectTimer;
    }

    if (!connectSuccess) {
        return false;
    }

    connect(this->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_websocket_error(QAbstractSocket::SocketError)));
    connect(this->socket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &message){
        QJsonObject obj = QJsonDocument::fromJson(message).object();
        QString method=obj["Method"].toString();
        QString packetType=obj["PacketType"].toString();
        if (packetType== "PtRpcReply")
        {
            RpcWaitObj* waitObj = this->rpcWaitMap[method];
            this->rpcWaitMap.erase(method);
            if (waitObj != NULL)
            {
                waitObj->Succed = waitObj->rpcRecv.DecodeFromQByteArray(message);
                waitObj->loop->quit();
            }
        }
        else if (packetType == "PtRpcNotice")
        {
            this->OnNoticeMessage(method, message);
        }
    });
    return true;
}

bool RpcHub::SendAndRecv(RpcAbstract &sendObj, RpcAbstract &recvObj)
{
    if (!this->socket->isValid() || socketIsError ) {
        return false;
    }

    QString method = sendObj.GetMethod();
    RpcWaitObj *waitObj=this->rpcWaitMap[method];
    if (waitObj != NULL) {
        return false;
    }
    bool ok = false;

    waitObj = new RpcWaitObj(recvObj);
    waitObj->loop = new QEventLoop;
    rpcWaitMap[method] = waitObj;
    qint64 dataLen = socket->sendBinaryMessage(sendObj.EncodeToQByteArray());
    if (dataLen < sendObj.EncodeToQByteArray().length()) {
        return false;
    }
    waitObj->loop->exec();
    ok = waitObj->Succed;
    delete waitObj->loop;
    delete waitObj;
    return ok;
}


RpcHub::~RpcHub()
{
    close_websocket();
}

void RpcHub::on_websocket_error(QAbstractSocket::SocketError)
{
    RemoveAll();
    emit this->signal_websocket_error(socket->errorString());
}

void RpcHub::RemoveAll()
{
    for (auto& a : this->rpcWaitMap)
    {
        a.second->loop->quit();
    }
    this->rpcWaitMap.clear();
    this->ClearNoticeCallback();
}

void RpcHub::close_websocket()
{
    if (this->socket->isValid()) {
        this->socket->close();
    }
    this->RemoveAll();
}

