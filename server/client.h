#ifndef CLIENT_H
#define CLIENT_H
#include "database.h"
#include <QWebSocket>

class Client : public QObject {
    Q_OBJECT

private:
    QMutex& clientMapLock;
    QHash<QString, Client *>& clientMap;
    QMultiHash<QString, QJsonObject>& imgJsonMap;
    Database& db;
    QWebSocket *socket;
    QString name = "";
    void handleJson(const QJsonObject& jsonObject);

public:
    Client(QWebSocket *socket, QHash<QString, Client *> &clientMap, QMultiHash<QString, QJsonObject>& imgJsonMap, Database& db, QMutex& clientMapLock);
    ~Client();
    void send(const QJsonObject&);

public slots:
    void onBinaryMessageReceived(const QByteArray& array);
    void onTextMessageReceived(const QString& str);
    void onDisconnected();

signals:
    void sendBinaryMessage(const QByteArray& array, QWebSocket *socket);
    void sendTextMessage(const QString& str, QWebSocket *socket);
    void close(QWebSocket *socket);
    void clear(Client *client);
};

#endif // CLIENT_H
