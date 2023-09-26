#include "clientsendthread.h"

ClientSendThread::ClientSendThread(const QString &path, const QString &serverIp, quint16 port, QObject *parent) : QThread{parent}, file(path), ip(serverIp), port(port) {

}

ClientSendThread::~ClientSendThread() {
    if (socket != nullptr) {
        socket->abort();
    }
    delete socket;
    file.close();
}

void ClientSendThread::run() {
    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::bytesWritten, this, &ClientSendThread::onBytesWritten, Qt::DirectConnection);
    connect(socket, &QTcpSocket::stateChanged, this, &ClientSendThread::onStateChanged, Qt::DirectConnection);
    socket->connectToHost(ip, port);
    exec();
}

void ClientSendThread::onBytesWritten(qint64) {
    int len = file.read(buf, 1048576);
    if (len > 0) {
        alreadySize += len;
        socket->write(buf, len);
        emit progress(alreadySize * 100 / size);
    } else if (len < 0) {
        socket->abort();
    }
}

void ClientSendThread::onStateChanged() {
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        if (alreadySize != size) {
            emit fail();
        } else {
            emit success();
        }
        file.close();
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        file.open(QIODevice::ReadOnly);
        size = file.size();
        socket->write(reinterpret_cast<char *>(&size), 8);
    }
}
