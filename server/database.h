#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

struct User {
    QString name;
    QString pwdHash;
};

struct History {
    QString from;
    QString to;
    QString msg;
    quint8 fontSize;
    quint8 style;
    qint32 color;
    qint64 timestamp;
};

class Database {
public:
    explicit Database();
    ~Database();
    void openDatabase();    //检测数据库是否开启，没启动则自动启动
    void closeDatabase();   //关闭数据库
    bool createTable();     //创建3个数据表，只需要运行一次
    bool insertUser(QString, QString, QReadWriteLock &);     //插入用户信息(username, passwordhash)
    User selectUser(QString, QReadWriteLock &);      //通过用户对象查询用户信息,返回用户对象
    bool deleteUser(QString, QReadWriteLock &);
    bool insertHistory(QString, QString, QString, int, int, int, qint64, QReadWriteLock &);  //插入聊天记录(发送方，接收方，消息，字号，样式，颜色，时间)
    QVector<History> selectLatestHistory(QString, QString, QReadWriteLock &);             //通过两个uesrname(QString)查询聊天记录，自动根据时间排序，username之间无顺序要求
    QVector<History> selectLatestGroupHistory(QString, QReadWriteLock &);                 //查询某个群的聊天记录
    QVector<History> selectHistory(QString, QString, qint64, qint64, QReadWriteLock &);   //查询某一时间段的好友聊天记录
    QVector<History> selectGroupHistory(QString, qint64, qint64, QReadWriteLock &);       //查询某一时间段的群聊记录
    bool deleteGroupHistory(QString, QReadWriteLock &);
    bool insertFriend(QString, QString, QReadWriteLock &);    //输入两个username插入friend表，顺序无所谓
    bool deleteFriend(QString, QString, QReadWriteLock &);    //删除好友，顺序无所谓
    QVector<QString> selectFriend(QString, QReadWriteLock &);      //查找输入username的全部好友

private:
    QSqlDatabase m_db;
};

#endif // DATABASE_H
