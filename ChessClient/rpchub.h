#ifndef RPCHUB_H
#define RPCHUB_H

#include <map>
#include <QObject>
#include <QString>
#include <QAbstractSocket>
#include "CSProtocol.h"

class QEventLoop;
class QWebSocket;

struct RpcWaitObj
{
    QEventLoop *loop;
    RpcAbstract& rpcRecv;
    bool Succed;
public :
    RpcWaitObj(RpcAbstract& rcv)
        : loop(NULL), rpcRecv(rcv), Succed(false){}
};

class RpcHub
        : public QObject, public NoticeBase
{
    Q_OBJECT
signals:
    void signal_websocket_error(QString);
public:
    RpcHub();
    ~RpcHub();
    bool InitWebsocket(const QString &wsUrl, int timeoutSecond);
    bool SendAndRecv(RpcAbstract& sendObj, RpcAbstract& recvObj);
    void RemoveAll();
public slots:
    void close_websocket();
private:
    bool connectToWs(const QString& wsUrl, int timeoutSecond);
    RpcHub(const RpcHub&) = delete;
    RpcHub& operator =(const RpcHub&) = delete;
private:
    QWebSocket *socket;
    std::map<QString, RpcWaitObj*> rpcWaitMap;
};

#endif // RPCHUB_H
