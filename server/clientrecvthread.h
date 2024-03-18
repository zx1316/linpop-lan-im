#ifndef CLIENTRECVTHREAD_H
#define CLIENTRECVTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QFile>

/**
 * - 以事件循环为核心的作客户端的文件接收线程
 * - 构造函数参数：保存文件路径、服务端IP、服务端端口
 * - 信号：
 * - success() 接收成功
 * - fail() 接受失败
 * - progress(qint8) 接收进度，每接收1MB数据发出一次，1~100的整数
 * - 注意事项：
 * - 父线程在任何时候都可以安全地对该线程quit、wait、deleteLater一键三连，无论是否正在接收文件
 * - 不要忘记最后要对该线程指针一键三连！
 * - 该线程没有看门狗功能，如需要请在父线程设置，用progress信号喂狗，如果超时可以直接一键三连
 */
class ClientRecvThread : public QThread {
    Q_OBJECT

public:
    explicit ClientRecvThread(const QString& path, const QString& serverIp, quint16 port, QObject *parent = nullptr);
    ~ClientRecvThread();
    void run() override;

private:
    char buf[1048576];
    QFile file;
    QTcpSocket *socket = nullptr;
    QString ip;
    qint64 size = -1;
    qint64 alreadySize = 0;
    quint16 port;

private slots:
    void onReadyRead();
    void onStateChanged();

signals:
    void success();
    void fail();
    void progress(qint8);
};

#endif // CLIENTRECVTHREAD_H
