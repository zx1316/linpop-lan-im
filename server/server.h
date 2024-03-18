#ifndef SERVER_H
#define SERVER_H
#include "database.h"
#include "client.h"
#include <QWebSocketServer>

class Server : public QObject {
    Q_OBJECT

private:
    QWebSocketServer serverSocket;
    QMutex clientMapLock;
    QHash<Client *, QThread *> threadMap;
    QHash<QString, Client *> clientMap;
    QMultiHash<QString, QJsonObject> imgJsonMap;
    Database db;

public:
    Server(quint16 port);
    ~Server();

public slots:
    void onNewConnection();
    void toSendTextMessage(const QString& str, QWebSocket *socket);
    void toSendBinaryMessage(const QByteArray& array, QWebSocket *socket);
    void toClose(QWebSocket *socket);
    void onClear(Client *client);
};

#endif // SERVER_H
