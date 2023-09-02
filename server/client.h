#ifndef CLIENT_H
#define CLIENT_H
#include "friendlybufferhelper.h"
#include "database.h"
#include <QTcpSocket>
#include <QTcpServer>

class Client : public QObject {
    Q_OBJECT

private:
    QHash<QString, Client *> &clientMap;
    Database &db;
    QReadWriteLock &lock;
    QString name = "";
    QTcpSocket *const socket;
    static constexpr quint8 S_ACCEPT_FILE = 0;
    static constexpr quint8 S_REJECT_FILE = 1;
    static constexpr quint8 S_MAKE_GROUP = 2;
    static constexpr quint8 S_ADD_FRIEND = 3;
    static constexpr quint8 S_DELETE_FRIEND = 4;
    static constexpr quint8 S_SEARCH_RECORD = 5;
    static constexpr quint8 S_SEARCH_RECORD_WITH_TIME = 6;
    static constexpr quint8 S_REGISTER = 7;
    static constexpr quint8 S_LOGIN = 8;
    static constexpr quint8 S_REQUEST_FILE = 9;
    static constexpr quint8 S_REQUEST_FRIEND = 10;
    static constexpr quint8 S_MESSAGE = 11;
    static constexpr quint8 C_ACCEPT_FILE = 0;
    static constexpr quint8 C_REJECT_FILE = 1;
    static constexpr quint8 C_REGISTER_SUCCESS = 2;
    static constexpr quint8 C_FRIEND_LIST = 3;
    static constexpr quint8 C_RECORD = 4;
    static constexpr quint8 C_LOGIN_SUCCESS = 5;
    static constexpr quint8 C_MESSAGE_SUCCESS = 6;
    static constexpr quint8 C_ADD_FRIEND_SUCCESS = 7;
    static constexpr quint8 C_DELETE_FRIEND_SUCCESS = 8;
    static constexpr quint8 C_REQUEST_FILE = 9;
    static constexpr quint8 C_MAKE_GROUP_SUCCESS = 10;
    static constexpr quint8 C_MESSAGE = 11;
    static constexpr quint8 C_ERR_REGISTER_NAME = 12;
    static constexpr quint8 C_ERR_LOGIN_NAME_PWD = 13;
    static constexpr quint8 C_ERR_LOGIN_ALERADY_IN = 14;
    static constexpr quint8 C_ERR_GROUP_CREATE = 15;
    static constexpr quint8 C_ERR_ALREADY_FRIEND = 16;
    static constexpr quint8 C_ERR_FRIEND_NOT_EXIST = 17;
    static constexpr quint8 C_ERR_MESSAGE = 18;
    static constexpr quint8 C_FRIEND_ONLINE = 19;
    static constexpr quint8 C_FRIEND_OFFLINE = 20;
    static constexpr quint8 C_BEING_ADDED = 21;
    static constexpr quint8 C_BEING_DELETED = 22;
    int makeFriendPacket(FriendlyBufferHelper *helper, QVector<QString> &friends, int start);
//    void sendFriendPacket(QString name, FriendlyBufferHelper *helper);

public:
    Client(QTcpSocket *socket, QHash<QString, Client *> &clientMap, Database &db, QReadWriteLock &lock);
    ~Client();
    void send(char *sendArr);

public slots:
    void onReadyRead();
    void onDisconnected();

signals:
    void writeData(char *dat, QTcpSocket *socket);
//    void writeDataAndClose(char *dat, QTcpSocket *socket);
    void close(QTcpSocket *socket);
    void offline(Client *client);
};

#endif // CLIENT_H
