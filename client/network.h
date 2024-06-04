#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QWebSocket>
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
    QWebSocket socket;
    QMultiHash<QString, QJsonObject> imgJsonMap;
    QHash<QString, ChatRecord> msgCache;
    QString serverIp;
    quint16 serverPort = 8848;
    explicit Network(QObject *parent = nullptr);
    void handleJson(const QJsonObject& obj);
    void writeJson(const QJsonObject& obj);
    void requestImg(const QString& imgName, const QJsonObject& obj);

public:
    static Network& getInstance();
    Network(const Network&) = delete;
    Network& operator=(const Network&) = delete;
    ~Network();
    void setIpAndPort(const QString& ip, quint16 port);
    void connectToServer();
    void disconnectFromServer();
    void requestRegister(const QString &name, const QString &pwdHash, const QString &imgBase64);
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
    QString getIp() const;
    quint16 getPort() const;
    QString getLocalIp() const;
    bool isDisconnected() const;

private slots:
    void onBinaryMessageReceived(const QByteArray& array);
    void onTextMessageReceived(const QString& str);
    void onStateChanged();

signals:
    void registerSuccessSignal();
    void registerFailSignal();
    void loginSuccessSignal(const QString& imgName, const QList<User>& list);
    void loginUnauthorizedSignal();
    void loginAlreadySignal();
    void groupMemberListSignal(const QString& groupName, const QList<QString>& list);
    void createGroupSuccessSignal();
    void createGroupFailSignal();
    void addFriendSuccessSignal(const QString& name, const QString& ip, const QString& imgName);
    void addFriendFailSignal();
    void beAddedSignal(const QString& name, const QString& ip, const QString& imgName);
    void beDeletedSignal(const QString& name);
    void friendOnlineSignal(const QString& name, const QString& ip);
    void friendOfflineSignal(const QString& name);
    void friendImageChangedSignal(const QString& name, const QString& imgName);
    void newMsgSignal(const QString& innerName, const QString& sender, const QString& msg, const QString& type);
    void requestFileSignal(const QString& sender, const QString& fileName, qint64 size);
    void acceptFileSignal(const QString& receiver, quint16 port);
    void rejectFileSignal(const QString& receiver);
    void historySignal(const QString& name, const QList<ChatRecord>& list);
    void fileListSignal(const QString& groupName, const QList<GroupFile>& list);
    void sendMsgSuccessSignal(const QString& name, const QString& msg, const QString& type);
    void disconnectedSignal();
    void connectedSignal();
};

#endif // NETWORK_H
