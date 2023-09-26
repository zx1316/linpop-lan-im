#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

struct User {
    QString name;
    QString img;
    QString ip;
    bool isNewMsg;
};

struct ChatRecord {
    QString sender;
    QString msg;
    QString type;
    qint64 timestamp;
};

struct GroupFile {
    QString name;
    QString uploader;
    qint64 size;
    qint64 timestamp;
};

// 常驻内存网络通信专用对象
class Network : public QObject {
    Q_OBJECT

private:
    QTimer timer;
    QTcpSocket socket;
    QMultiHash<QString, QJsonObject> imgJsonMap;
    QHash<QString, ChatRecord> msgCache;
    QString serverIp;
    char *recvArr = new char[4 * 1024 * 1024];
    qint32 recvLen = 0;
    qint32 exceptLen = 4;
    quint16 serverPort = 8848;
    bool isReadyReadJson = false;
    bool isCompressed;
    void handleJson(const QJsonObject &obj);
    void writeJson(const QJsonObject &obj);
    void requestImg(const QString &imgName, const QJsonObject &obj);

public:
    explicit Network(QObject *parent = nullptr);
    ~Network();
    void setIpAndPort(const QString &ip, quint16 port) {
        serverIp = ip;
        serverPort = port;
    }
    void connectToServer() {
        socket.connectToHost(serverIp, serverPort);
    }
    void disconnectFromServer() {
        socket.disconnectFromHost();
    }
    void requestRegister(const QString &name, const QString &pwdHash, const QString &imgName);
    void requestLogin(const QString &name, const QString &pwdHash);
    void requestGroupMemberList(const QString &name);
    void requestCreateGroup(const QString &groupName, const QString &imgName, const QList<QString> &memberList);
    void requestAddFriend(const QString &name);
    void requestDeleteFriend(const QString &name);
    void requestSendMsg(const QString &receiver, const QString &msg, const QString &type);
    void requestLatestHistory(const QString &name);
    void requestHistory(const QString &name, const QDate &start, const QDate &end);
    void requestRequestFile(const QString &receiver, const QString &fileName, qint64 size);
    void requestAcceptFile(const QString &sender, quint16 port);
    void requestRejectFile(const QString &sender);
    void requestChangeImg(const QString &imgName);
    void requestGroupFileList(const QString &groupName);
    void requestUploadFile(const QString &groupName, const QString &fileName, qint64 size, quint16 port);
    void requestDownloadFile(const QString &groupName, const QString &fileName, quint16 port);
    void requestDeleteFile(const QString &groupName, const QString &fileName);
    QString getIp() {
        return serverIp;
    }
    quint16 getPort() {
        return serverPort;
    }
    QString getLocalIp() {
        return socket.localAddress().toString();
    }

private slots:
    void onReadyRead();
    void onStateChanged();

signals:
    void registerSuccessSignal();
    void registerFailSignal();
    void loginSuccessSignal(QString imgName, QList<User> list);
    void loginUnauthorizedSignal();
    void loginAlreadySignal();
    void groupMemberListSignal(QString groupName, QList<QString> list);
    void createGroupSuccessSignal();
    void createGroupFailSignal();
    void addFriendSuccessSignal(QString name, QString ip, QString imgName);
    void addFriendFailSignal(QString name);
    void beAddedSignal(QString name, QString ip, QString imgName);
    void beDeletedSignal(QString name);
    void friendOnlineSignal(QString name, QString ip);
    void friendOfflineSignal(QString name);
    void friendImageChangedSignal(QString name, QString imgName);
    void newMsgSignal(QString innerName, QString sender, QString msg, QString type);
    void requestFileSignal(QString sender, QString fileName, qint64 size);
    void acceptFileSignal(QString receiver, quint16 port);
    void rejectFileSignal(QString receiver);
    void historySignal(QString name, QList<ChatRecord> list);
    void fileListSignal(QString groupName, QList<GroupFile> list);
    void sendMsgSuccessSignal(QString name, QString msg, QString type);
    void disconnectedSignal();
    void connectedSignal();
};

#endif // NETWORK_H
