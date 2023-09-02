#ifndef SERVER_H
#define SERVER_H
#include "database.h"
#include "client.h"
#include <QTcpServer>

class Server : public QObject {
    Q_OBJECT

private:
    QTcpServer *serverSocket;
    QHash<Client *, QThread *> threadMap;
    QHash<QString, Client *> clientMap;
    Database db;
    QReadWriteLock lock;

public:
    Server(quint16 port);
    virtual ~Server();

public slots:
    void onNewConnection();
    void toWriteData(char *dat, QTcpSocket *socket);
//    void toWriteDataAndClose(char *dat, QTcpSocket *socket);
    void toClose(QTcpSocket *socket);
    void onOffline(Client *client);
};

#endif // SERVER_H
