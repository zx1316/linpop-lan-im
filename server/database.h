#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

struct User {
    QString name;
    QString pwd;
    QString imgName;
    bool isNewMsg;
};

struct ChatRecord {
    QString sender;
    QString receiver;
    QString msg;
    QString type;
    qint64 timestamp;
};

struct GroupFile {
    QString groupName;
    QString fileName;
    QString uploader;
    qint64 size;
    qint64 timestamp;
};

class Database {
public:
    Database();
    ~Database();
    void createTable();     //创建数据表，只需要运行一次
    int insertUser(const QString& name, const QString& pwd, const QString& imgName);     //插入用户信息
    User selectUser(const QString& name);      //通过用户对象查询用户信息,返回用户对象
    bool updateUserImg(const QString& name, const QString& imgName);
    bool insertChatRecord(const QString& sender, const QString& receiver, const QString& msg, const QString& type, qint64 timestamp);  //插入聊天记录(发送方，接收方，消息，时间，字体，颜色，字号，样式)
    QList<ChatRecord> selectChatRecords(const QString& name1, const QString& name2, qint64 start, qint64 end, quint16 limit);
    bool insertFriend(const QString& name1, const QString& name2);    //输入两个username插入friend表，顺序无所谓，群必须放第二个参数
    bool deleteFriend(const QString& name1, const QString& name2);    //删除好友，顺序无所谓
    QList<User> selectFriends(const QString& name);      //查找输入username的全部好友
    bool insertImg(const QString& imgName, const QByteArray& data);
    QByteArray selectImg(const QString& imgName);
    bool insertFile(const QString& groupName, const QString& fileName, const QString& uploader, qint64 size, qint64 timestamp);
    GroupFile selectFile(const QString& groupName, const QString& fileName);
    QList<GroupFile> selectFiles(const QString& groupName);
    bool deleteFile(const QString& groupName, const QString& fileName);
    bool updateOfflineTime(const QString& name, qint64 timestamp);
    bool isOpen() const {
        return m_db.isOpen();
    }

private:
    QSqlDatabase m_db;
    QReadWriteLock lock;
};

#endif // DATABASE_H
