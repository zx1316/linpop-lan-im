#ifndef SERVER_H
#define SERVER_H
#include "database.h"
#include "client.h"
#include <QTcpServer>

class Server : public QObject {
    Q_OBJECT

private:
    QTcpServer serverSocket;
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
    void toWriteData(QByteArray array, QTcpSocket *socket);
    void toClose(QTcpSocket *socket);
    void onClear(Client *client);
};

#endif // SERVER_H
