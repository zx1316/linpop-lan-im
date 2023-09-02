#include "receivefilebackground.h"
#include <QDebug>

ReceiveFileBackground::ReceiveFileBackground(
        QString file_name,
        quint64 file_size,
        quint16 port,
        QObject *parent) :
    QObject(parent),
    _file_name(file_name),
    _file_size(file_size),
    _port(port),
    _receive_size(0)
{
    _file.setFileName("./" + _file_name);
    _file.moveToThread(thread());
    _file.open(QIODevice::WriteOnly|QIODevice::Append);
}

/**
 * 函数名称：startServer()
 * 描述：开启服务器，监听端口
 * 参数：void
 * 返回值:void
 * 做成时间：2023.8.27
 * 作者：刘文景
 */
void ReceiveFileBackground::startServer(){
    _server = new QTcpServer;
    _server->moveToThread(thread());
    connect(_server, SIGNAL(newConnection()),
            this, SLOT(newConnection_slot()));
    _server->listen(QHostAddress::Any, _port);
}

/**
 * 函数名称：newConnection_slot()
 * 描述：与文件发送端取得链接
 * 参数：void
 * 返回值:void
 * 做成时间：2023.8.27
 * 作者：刘文景
 */
void ReceiveFileBackground::newConnection_slot(){
    _socket = _server->nextPendingConnection();
    _socket->moveToThread(thread());
    connect(_socket, SIGNAL(readyRead()),
            this, SLOT(readyRead_slot()),
            Qt::DirectConnection);
    qDebug()<<"连接成功";
    emit getConnect();
}

/**
 * 函数名称：readyRead_slot()
 * 描述：收取文件信息,修改了文件包构成，并在文件收取结束后向主线程发送信号
 * 参数：void
 * 返回值:void
 * 做成时间：2023.8.29
 * 作者：刘文景
 */
void ReceiveFileBackground::readyRead_slot(){
    /**
     * uint32 : id 	4byte
     * uint32 : length	4byte
     * char[1024] : info	1024byte
     **/
    _message += _socket->readAll();
    qDebug()<<"readyRead_slot()";
    if(_message.size() < 1032){
        return;
    }
    while(1){
        QByteArray package = _message.mid(0, 1032);
        _message = _message.mid(1032);
        qint32 data_length = (package[4] << 24)
                + (package[5] << 16)
                + (package[6] << 8)
                + package[7];
        if(data_length > 0){
            QByteArray file_package = package.mid(8, data_length);
            _file.write(file_package, data_length);
            _receive_size += data_length;
            qDebug()<<"message.size: "<<_message.size();
            qDebug()<<"_receive_size: "<<_receive_size;
            emit receiving(_receive_size);
        }
        else if(_receive_size == _file_size){
            //接收成功
            _file.close();
            //选择相应的目录保存
            QString filename;
            QWidget *qwidget = new QWidget();
            filename = QFileDialog::getSaveFileName(
                        qwidget, "choose file",
                        "./"+_file_name, nullptr);
            QDir d;
            d.rename("./"+_file_name,filename);
            _socket->close();
            _server->close();
            emit receiveSuccess();
            break;
        }
        if(_message.size() < 1032){
            break;
        }
    }

}
