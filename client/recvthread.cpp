#include "recvthread.h"

RecvThread::RecvThread(QString path) : path(path) {

}

RecvThread::~RecvThread() {
    qDebug() << "destructing receive thread";
}

void RecvThread::run() {
    // 1：发送方发送 总大小(int64)
    // 2：发送方发送 数据(数组最大1MB)
    // 3：接收方读取所有数据后主动断开连接即可
    int percent = 0;
    char readBuffer[BUF_SIZE];
    QTcpServer server;
    qint32 port;
    for (port = 10000; port < 65536; port++) {
        if (server.listen(QHostAddress::AnyIPv4, port)) {
            break;
        }
    }
    emit availablePort(port);

    if (!server.waitForNewConnection(30000)) {
        emit failed();
        return;
    }

    QFile qFile(path);
    if (!qFile.open(QFile::WriteOnly | QFile::Truncate)) {
         emit failed();
         return;
    }

    QTcpSocket *client = server.nextPendingConnection();
    qDebug() << "file opened";
    qint64 totalLen = 0, alreadyLen = 0;
    qint32 readLen;
    while (!interruptFlag) {
        if (!client->waitForReadyRead()) {
            qFile.close();
            client->disconnectFromHost();
            delete client;
            emit failed();
            return;
        }
        while (true) {
            if (totalLen == 0) {
                client->read(readBuffer, 8);
                totalLen = *(qint64 *) readBuffer;
            } else {
                readLen = client->read(readBuffer, BUF_SIZE);
                if (readLen == -1) {
                    qFile.close();
                    client->disconnectFromHost();
                    delete client;
                    emit failed();
                    return;
                }
                if (readLen == 0) {
                    break;
                }
                if (qFile.write(readBuffer, readLen) != readLen) {
                    qFile.close();
                    client->disconnectFromHost();
                    delete client;
                    emit failed();
                    return;
                }
                alreadyLen += readLen;
                int percent1 = (int) ((double) alreadyLen / (double) totalLen * 100.0);
                if (percent1 != percent) {
                    emit progress(percent1);
                    percent = percent1;
                }
                if (alreadyLen == totalLen) {
                    // 接收完毕喽
                    qFile.close();
                    client->disconnectFromHost();
                    delete client;
                    emit finish();
                    return;
                }
            }
        }
    }
    qFile.close();
    client->disconnectFromHost();
    delete client;
    emit failed();
}
