#ifndef CLIENT_H
#define CLIENT_H
#include "database.h"
#include <QTcpSocket>

class Client : public QObject {
    Q_OBJECT

private:
    QMutex &clientMapLock;
    QHash<QString, Client *> &clientMap;
    QMultiHash<QString, QJsonObject> &imgJsonMap;
    Database &db;
    QTcpSocket * const socket;
    QString name = "";
    char *recvArr = new char[2 * 1024 * 1024];
    qint32 recvLen = 0;
    qint32 exceptLen = 4;
    bool isReadyReadJson = false;
    bool isCompressed;
    void handleJson(const QJsonObject &jsonObject);

public:
    Client(QTcpSocket *socket, QHash<QString, Client *> &clientMap, QMultiHash<QString, QJsonObject> &imgJsonMap, Database &db, QMutex &clientMapLock);
    ~Client();
    void send(const QJsonObject &);

public slots:
    void onReadyRead();
    void onDisconnected();

signals:
    void writeData(QByteArray array, QTcpSocket *socket);
    void close(QTcpSocket *socket);
    void clear(Client *client);
};

#endif // CLIENT_H
