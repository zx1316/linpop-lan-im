#ifndef RECVTHREAD_H
#define RECVTHREAD_H
#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include "friendlybufferhelper.h"

class RecvThread : public QThread {
    Q_OBJECT
public:
    RecvThread(QString path);
    virtual ~RecvThread();
    void run() override;
    volatile bool interruptFlag = false;
private:
    QString path;
    static constexpr int BUF_SIZE = 1048576;
signals:
    void progress(int);
    void noRequest();
    void failed();
    void availablePort(int);
    void finish();
};

#endif // RECVTHREAD_H8
