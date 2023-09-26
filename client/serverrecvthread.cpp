#include "serverrecvthread.h"

ServerRecvThread::ServerRecvThread(const QString &path, const QString &clientIp, QObject *parent) : QThread{parent}, file(path), ip(clientIp) {

}

ServerRecvThread::~ServerRecvThread() {
    if (server != nullptr) {
        server->close();
    }
    delete server;     // delete server后会自动delete socket
    file.close();
}

void ServerRecvThread::run() {
    server = new QTcpServer;
    for (int i = 49152; i < 65536; i++) {
        if (server->listen(QHostAddress::AnyIPv4, i)) {
            emit portAvailable(i);
            connect(server, &QTcpServer::newConnection, this, &ServerRecvThread::onNewConnection, Qt::DirectConnection);
            exec();
            return;
        }
    }
    emit fail();
    delete server;
    server = nullptr;
}

void ServerRecvThread::onNewConnection(){
    auto socket1 = server->nextPendingConnection();
    if (socket == nullptr && socket1->peerAddress().toString() == ip) {
        socket = socket1;
        connect(socket, &QTcpSocket::disconnected, this, &ServerRecvThread::onDisconnected, Qt::DirectConnection);
        connect(socket, &QTcpSocket::readyRead, this, &ServerRecvThread::onReadyRead, Qt::DirectConnection);
        file.open(QIODevice::WriteOnly);
    } else {
        socket1->abort();
        delete socket1;
    }
}

void ServerRecvThread::onReadyRead() {
    while (true) {
        if (size == -1) {
            socket->read(reinterpret_cast<char *>(&size), 8);
            if (size == 0) {
                socket->disconnectFromHost();
                break;
            }
        } else {
            int len = socket->read(buf, 1048576);
            if (len <= 0) {
                break;
            }
            if (file.write(buf, len) != len) {
                socket->abort();
                break;
            }
            alreadySize += len;
            emit progress(alreadySize * 100 / size);
            if (alreadySize == size) {
                socket->disconnectFromHost();
                break;
            }
        }
    }
}

void ServerRecvThread::onDisconnected() {
    if (alreadySize == size) {
        emit success();
    } else {
        emit fail();
    }
    server->close();
    file.close();
}
