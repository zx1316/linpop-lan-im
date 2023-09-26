#include "server.h"

Server::Server(quint16 port) {
    if (!db.isOpen()) {
        qDebug() << "Failed to open database.";
        exit(1);
    }
    db.createTable();
    if (!serverSocket.listen(QHostAddress::AnyIPv4, port)) {
        qDebug() << "Failed to start server." << serverSocket.errorString();
        exit(1);
    }
    qDebug() << "Server started.";
    QDir dir(QCoreApplication::applicationDirPath());
    dir.mkdir("group_files");
    connect(&serverSocket, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    serverSocket.close();
}

void Server::onNewConnection() {
    QThread *thread = new QThread;
    QTcpSocket *socket = serverSocket.nextPendingConnection();
    Client *client = new Client(socket, clientMap, imgJsonMap, db, clientMapLock);
    qDebug() << "new connection. ip:" << socket->peerAddress();
    client->moveToThread(thread);
    connect(socket, &QTcpSocket::readyRead, client, &Client::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, client, &Client::onDisconnected);
    connect(client, &Client::writeData, this, &Server::toWriteData);
    connect(client, &Client::close, this, &Server::toClose);
    connect(client, &Client::clear, this, &Server::onClear);
    connect(thread, &QThread::finished, client, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    threadMap[client] = thread;
    thread->start();
}

void Server::onClear(Client *client) {
    QThread *thread = threadMap[client];
    thread->quit();
    threadMap.remove(client);
}

void Server::toWriteData(QByteArray array, QTcpSocket *socket) {
    socket->write(array);
}

void Server::toClose(QTcpSocket *socket) {
    socket->disconnectFromHost();
}

