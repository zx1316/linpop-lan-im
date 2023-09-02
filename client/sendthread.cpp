#include "sendthread.h"

SendThread::SendThread(QString ip, quint16 port, QString path) : ip(ip), port(port), path(path) {

}

SendThread::~SendThread() {
    qDebug() << "destructing send thread.";
}

void SendThread::run() {
    int percent = 0;
    char sendBuffer[BUF_SIZE];
    QTcpSocket client;
    client.connectToHost(ip, port);
    if (!client.waitForConnected(30000)) {
        qDebug() << "failed1";
        emit failed();
        return;
    }
    QFile qFile(path);
    if (!qFile.open(QIODevice::ReadOnly)) {
        qDebug() << "failed2";
        emit failed();
        return;
    }
    qint64 alreadyLen = 0;
    qint64 size = qFile.size();
    qDebug() << "sender file opened size" << size;
    *(qint64 *) sendBuffer = size;
    client.write(sendBuffer, 8);
    while (!interruptFlag) {
        int len = qFile.read(sendBuffer, BUF_SIZE);
        if (len == 0) {
            qFile.close();
            client.waitForDisconnected();
            emit finish();
            return;
        }
        alreadyLen += len;
        if (client.write(sendBuffer, len) != len) {
            qDebug() << "failed3";
            qFile.close();
            client.disconnectFromHost();
            emit failed();
            return;
        }
        int percent1 = (int) ((double) alreadyLen / (double) size * 100.0);
        if (percent1 != percent) {
            emit progress(percent1);
            percent = percent1;
        }
        client.waitForBytesWritten();    // 有效
    }
    qDebug() << "failed4";
    qFile.close();
    client.disconnectFromHost();
    emit failed();
    return;
}
