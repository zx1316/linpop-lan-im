#ifndef CLIENTSENDTHREAD_H
#define CLIENTSENDTHREAD_H

#include <QThread>
#include <QFile>
#include <QTcpSocket>

/**
 * - 以事件循环为核心的作客户端的文件发送线程
 * - 构造函数参数：发送文件路径、服务端IP、服务端端口
 * - 信号：
 * - success() 发送成功
 * - fail() 发送失败
 * - progress(qint8) 发送进度，每发送1MB数据发出一次，1~100的整数
 * - 注意事项：
 * - 父线程在任何时候都可以安全地对该线程quit、wait、deleteLater一键三连，无论是否正在发送文件
 * - 不要忘记最后要对该线程指针一键三连！
 * - 该线程没有看门狗功能，如需要请在父线程设置，用progress信号喂狗，如果超时可以直接一键三连
 */
class ClientSendThread : public QThread {
    Q_OBJECT

public:
    explicit ClientSendThread(const QString& path, const QString& serverIp, quint16 port, QObject *parent = nullptr);
    ~ClientSendThread();
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
    void onBytesWritten(qint64);
    void onStateChanged();

signals:
    void success();
    void fail();
    void progress(qint8);
};

#endif // CLIENTSENDTHREAD_H
