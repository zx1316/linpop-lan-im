#include "serversendthread.h"

ServerSendThread::ServerSendThread(const QString& path, const QString& clientIp, QObject *parent) : QThread{parent}, file(path), ip(clientIp) {

}

ServerSendThread::~ServerSendThread() {
    if (server != nullptr) {
        server->close();
    }
    delete server;     // delete server后会自动delete socket
    file.close();
}

void ServerSendThread::run() {
    server = new QTcpServer;
    for (int i = 49152; i < 65536; i++) {
        if (server->listen(QHostAddress::AnyIPv4, i)) {
            emit portAvailable(i);
            connect(server, &QTcpServer::newConnection, this, &ServerSendThread::onNewConnection, Qt::DirectConnection);
            exec();
            return;
        }
    }
    emit fail();
    delete server;
    server = nullptr;
}

void ServerSendThread::onNewConnection() {
    auto socket1 = server->nextPendingConnection();
    if (socket == nullptr && socket1->peerAddress().toString() == ip) {
        socket = socket1;
        connect(socket, &QTcpSocket::disconnected, this, &ServerSendThread::onDisconnected, Qt::DirectConnection);
        connect(socket, &QTcpSocket::bytesWritten, this, &ServerSendThread::onBytesWritten, Qt::DirectConnection);
        file.open(QIODevice::ReadOnly);
        if (!file.isOpen()) {
            size = -1;
        } else {
            size = file.size();
        }
        socket->write(reinterpret_cast<char *>(&size), 8);
    } else {
        socket1->abort();
        delete socket1;
    }
}

void ServerSendThread::onBytesWritten(qint64) {
    int len = file.read(buf, 1048576);
    if (len > 0) {
        alreadySize += len;
        socket->write(buf, len);
        emit progress(alreadySize * 100 / size);
    } else if (len < 0) {
        socket->abort();
    }
}

// 最后client确定接收完毕，主动断开连接
void ServerSendThread::onDisconnected() {
    if (alreadySize == size) {
        emit success();
    } else {
        emit fail();
    }
    server->close();
    file.close();
}
