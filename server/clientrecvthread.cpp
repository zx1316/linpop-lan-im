#include "clientrecvthread.h"

ClientRecvThread::ClientRecvThread(const QString &path, const QString &serverIp, quint16 port, QObject *parent) : QThread{parent}, file(path), ip(serverIp), port(port) {

}

ClientRecvThread::~ClientRecvThread() {
    if (socket != nullptr) {
        socket->abort();
    }
    delete socket;
    file.close();
}

void ClientRecvThread::run() {
    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::readyRead, this, &ClientRecvThread::onReadyRead, Qt::DirectConnection);
    connect(socket, &QTcpSocket::stateChanged, this, &ClientRecvThread::onStateChanged, Qt::DirectConnection);
    socket->connectToHost(ip, port);
    exec();
}

void ClientRecvThread::onReadyRead() {
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

void ClientRecvThread::onStateChanged() {
    if (socket->state() == QAbstractSocket::UnconnectedState) {
        if (alreadySize != size) {
            file.remove();
            emit fail();
        } else {
            file.close();
            emit success();
        }
    } else if (socket->state() == QAbstractSocket::ConnectedState) {
        file.open(QIODevice::WriteOnly);
    }
}
