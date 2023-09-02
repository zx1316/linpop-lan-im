#ifndef RECEIVEFILEBACKGROUND_H
#define RECEIVEFILEBACKGROUND_H

#include <QObject>
#include "QTcpServer"
#include "QTcpSocket"
#include <QFile>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
#include <QFileDialog>
#include "mytcpsocket.h"


class ReceiveFileBackground : public QObject
{
    Q_OBJECT
    QTcpServer* _server;
    QTcpSocket* _socket;
    QString _sender_ip;

public:
    explicit ReceiveFileBackground(
            QString file_name,
            quint64 file_size,
            quint16 port,
            QObject *parent = nullptr);

public slots:
    void startServer();
    void newConnection_slot();
    void readyRead_slot();

private:
    QString _url;
    QFile _file;
    QString _file_name;
    quint64 _file_size;
    quint16 _port;
    quint64 _receive_size;
    QByteArray _message;

signals:
    void receiving(const int receive_size);
    void receiveSuccess();
    void receiveFail();
    void getConnect();
};

#endif // RECEIVEFILEBACKGROUND_H
