/**
 * 文件名称：filesendbackground.h
 * 类名称：FileSendBackground（文件发送后台）
 * 描述：使用Socket与文件接收者进行网络连接并发送文件（用于非主线程）
 * 成员变量：
 * 略：自己看吧，太困了不想写
 * 初版完成时间：2023.08.26 02:59
 * 做成时间：----。--。--
 * 作者：刘文景
 */
#ifndef FILESENDBACKGROUND_H
#define FILESENDBACKGROUND_H

#include <QObject>
//#include <QTcpSocket>
#include <QThread>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include "mytcpsocket.h"
class FileSendBackground : public QObject
{
    Q_OBJECT
    MyTcpSocket* _socket;
    QTimer *_timer;
public:
    explicit FileSendBackground(
            QString url,
            QString receiver_username,
            QString receiver_ip,
            int port,
            QObject *parent = nullptr);


public slots:
    void toSendFile();
    void connected_slot();

private:
    QString _url, _receiver_username, _receiver_ip;
    int _port;
    QFile _file;
    QString _file_name;
    unsigned long long _file_size;
    unsigned long long _send_size;
    QByteArray packaging(quint32 id, quint32 length, char* buffer);

signals:
    // 线程完成工作时发送的信号
    void resultReady();
    void sending(const int file_size, const int send_size);
    void sendSuccess();
    void sendFail();
};

#endif // FILESENDBACKGROUND_H
