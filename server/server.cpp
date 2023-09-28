#include "server.h"
#include <QWebSocket>

Server::Server(quint16 port) : serverSocket("LinpopServer", QWebSocketServer::NonSecureMode) {
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
    connect(&serverSocket, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    serverSocket.close();
}

void Server::onNewConnection() {
    QThread *thread = new QThread;
    QWebSocket *socket = serverSocket.nextPendingConnection();
    Client *client = new Client(socket, clientMap, imgJsonMap, db, clientMapLock);
    qDebug() << "new connection. ip:" << socket->peerAddress() << "thread" << QThread::currentThread();
    client->moveToThread(thread);
    connect(socket, &QWebSocket::binaryMessageReceived, client, &Client::onBinaryMessageReceived);
    connect(socket, &QWebSocket::textMessageReceived, client, &Client::onTextMessageReceived);
    connect(socket, &QWebSocket::disconnected, client, &Client::onDisconnected);
    connect(client, &Client::sendTextMessage, this, &Server::toSendTextMessage);
    connect(client, &Client::sendBinaryMessage, this, &Server::toSendBinaryMessage);
    connect(client, &Client::close, this, &Server::toClose);
    connect(client, &Client::clear, this, &Server::onClear);
    connect(thread, &QThread::finished, client, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    threadMap[client] = thread;
    thread->start();
}

void Server::onClear(Client *client) {
    threadMap[client]->quit();
    threadMap.remove(client);
}

void Server::toSendTextMessage(QString str, QWebSocket *socket) {
    socket->sendTextMessage(str);
}

void Server::toSendBinaryMessage(QByteArray array, QWebSocket *socket) {
    socket->sendBinaryMessage(array);
}

void Server::toClose(QWebSocket *socket) {
    socket->close();
}

