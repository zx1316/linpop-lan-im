#ifndef SERVERRECVTHREAD_H
#define SERVERRECVTHREAD_H

#include <QThread>
#include <QFile>
#include <QTcpServer>
#include <QTcpSocket>

/**
 * - 以事件循环为核心的作服务端的文件接收线程
 * - 构造函数参数：接收文件路径、客户端IP
 * - 信号：
 * - portAvailable(quint16) 服务器已经打开，发出正在监听的端口
 * - success() 接收成功
 * - fail() 接收失败
 * - progress(qint8) 发送进度，每接收1MB数据发出一次，1~100的整数
 * - 注意事项：
 * - 父线程在任何时候都可以安全地对该线程quit、wait、deleteLater一键三连，无论是否正在发送文件
 * - 不要忘记最后要对该线程的指针一键三连！
 * - 该线程没有看门狗功能，如需要请在父线程设置，用progress信号喂狗，如果超时可以直接一键三连
 */
class ServerRecvThread : public QThread {
    Q_OBJECT

public:
    explicit ServerRecvThread(const QString &path, const QString &clientIp, QObject *parent = nullptr);
    ~ServerRecvThread();
    void run() override;

private:
    char buf[1048576];
    QFile file;
    QTcpServer *server = nullptr;
    QTcpSocket *socket = nullptr;
    QString ip;
    qint64 size = -1;
    qint64 alreadySize = 0;

private slots:
    void onNewConnection();
    void onReadyRead();
    void onDisconnected();

signals:
    void portAvailable(quint16);
    void success();
    void fail();
    void progress(qint8);
};

#endif // SERVERRECVTHREAD_H
