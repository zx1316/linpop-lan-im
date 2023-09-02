#ifndef SENDTHREAD_H
#define SENDTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include "friendlybufferhelper.h"

class SendThread : public QThread
{
    Q_OBJECT
public:
    SendThread(QString ip, quint16 port, QString path);
    virtual ~SendThread();
    void run() override;
    volatile bool interruptFlag = false;
private:
    QString ip;
    quint16 port;
    QString path;
    static constexpr int BUF_SIZE = 1048576;
signals:
    void progress(int);
    void finish();
    void noServer();
    void failed();
};

#endif // SENDTHREAD_H

