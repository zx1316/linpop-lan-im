#include "server.h"

Server::Server(quint16 port) {
    db.openDatabase();
    db.createTable();
    serverSocket = new QTcpServer();
    if (!serverSocket->listen(QHostAddress::AnyIPv4, port)) {
        qDebug() << "Failed to start server." << serverSocket->errorString();
        exit(1);
    } else {
        qDebug() << "Server started.";
        connect(serverSocket, &QTcpServer::newConnection, this, &Server::onNewConnection);
    }
}

Server::~Server() {
    db.closeDatabase();
    serverSocket->close();
    disconnect(serverSocket);
    serverSocket->deleteLater();
}

void Server::onNewConnection() {
    QThread *thread = new QThread;
    QTcpSocket *socket = serverSocket->nextPendingConnection();
    Client *client = new Client(socket, clientMap, db, lock);
    qDebug() << "new connection. ip:" << socket->peerAddress() << "current thread:" << QThread::currentThread();
    client->moveToThread(thread);
    connect(socket, &QTcpSocket::readyRead, client, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, client, &Client::onDisconnected);
    connect(client, &Client::writeData, this, &Server::toWriteData);
    connect(client, &Client::close, this, &Server::toClose);
//    connect(client, &Client::writeDataAndClose, this, &Server::toWriteDataAndClose);
    connect(client, &Client::offline, this, &Server::onOffline);
    connect(thread, &QThread::finished, client, &QObject::deleteLater);
    threadMap[client] = thread;
    thread->start();
}

void Server::onOffline(Client *client) {
    QThread *thread = threadMap[client];
    thread->quit();
    thread->wait();
    threadMap.remove(client);
}

void Server::toWriteData(char *dat, QTcpSocket *socket) {
//    qDebug() << "send packet:" << QByteArray(dat, 1408);
    socket->write(dat, 1408);
    socket->flush();
    delete dat;
}

void Server::toClose(QTcpSocket *socket) {
    socket->disconnectFromHost();
}

