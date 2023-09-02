#include <QThread>
#include <QDebug>
#include <QHostAddress>
#include "tcpclient.h"

TcpClient::TcpClient(QObject *parent)
    : QTcpSocket(parent)
{
    //自动连接在信号发射时被识别为队列连接/信号在主线程发射
    connect(this, &TcpClient::SignalConnectToHost, this, [&](const QString & address, quint16 port) {
        //test record# in child thread id 20588
        qDebug("SlotConnectToHost ThreadID:%d", QThread::currentThreadId());
        //
        this->connectToHost(QHostAddress(address), port, QIODevice::ReadWrite);
    }, Qt::AutoConnection);

    //连接了TCP服务端
    QObject::connect(this, &QAbstractSocket::connected, this, [&]() {
        //test record# in child thread id 20588
        qDebug("SlotHasConnected ThreadID:%d", QThread::currentThreadId());
        //
        m_bOnLine = true;
    }, Qt::DirectConnection);

    //断开了TCP服务端
    QObject::connect(this, &QAbstractSocket::disconnected, this, [&]() {
        //test record# in child thread id 20588
        qDebug("SlotHasDisconnected ThreadID:%d", QThread::currentThreadId());
        //
        m_bOnLine = false;
    }, Qt::DirectConnection);

    //收到了TCP服务的数据
    QObject::connect(this, &QIODevice::readyRead, this, [&]() {
        //test record# in child thread id 20588
        qDebug("SlotIODeviceReadyRead ThreadID:%d", QThread::currentThreadId());
        //读取全部数据
        m_btaReceiveFromService.append(this->readAll());
        //
        int iFindPos = m_btaReceiveFromService.indexOf("\r\n");
        //检查分隔符
        while (-1 != iFindPos)
        {
            //分割数据流
            QString strPublish = m_btaReceiveFromService.left(iFindPos);
            //发布解析后的格式数据
            emit SignalPublishFormatRecvData(strPublish);
            //
            m_btaReceiveFromService.remove(0, iFindPos + strlen("\r\n"));
            //
            iFindPos = m_btaReceiveFromService.indexOf("\r\n");
        }
    }, Qt::DirectConnection);

    //执行数据发送过程
    QObject::connect(this, &TcpClient::SignalSendingData, this, [&](const QByteArray c_btaData) {
        //test record# in child thread id 20588
        qDebug("SlotSendingData ThreadID:%d", QThread::currentThreadId());
        //
        this->write(c_btaData);
    }, Qt::AutoConnection);
}

//
TcpClient::~TcpClient()
{
}

//跨线程转换
void TcpClient::ClientConnectToHost(const QString & address, quint16 port)
{
    emit SignalConnectToHost(address, port);
}

//跨线程转换
void TcpClient::ClientSendingData(const QByteArray & c_btaData)
{
    emit SignalSendingData(c_btaData);
}

//是否在线
bool TcpClient::IsOnline()
{
    return m_bOnLine;
}
