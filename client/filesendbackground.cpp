#include "filesendbackground.h"
#include <QtDebug>

FileSendBackground::FileSendBackground(
        QString url,
        QString receiver_username,
        QString receiver_ip,
        int port,
        QObject *parent) :
    QObject(parent), _url(url),
    _receiver_username(receiver_username),
    _receiver_ip(receiver_ip),
    _port(port)
{}
/**
 * 函数名称：connected_slot()
 * 描述：发送文件传输请求
 * 参数：void
 * 返回值:void
 * 做成时间：2023.8.29
 * 作者：刘文景
 */
void FileSendBackground::connected_slot(){
    QFileInfo info(_url);
    _file_name = info.fileName();
    _file_size = (long long)(info.size());
    _send_size = 0LL;
    _file.setFileName(_url);
    if(_file.open(QIODevice::ReadOnly)){
        /*QByteArray send_message;
        short data_length = 0;
        do{
            thread()->msleep(1);
            send_message.clear();
            char buffer[1024] = {0};
            data_length = _file.read(buffer, sizeof(buffer));
            send_message.append(char(10))
                    .append(char(_receiver_username.length()))
                    .append(_receiver_username)
                    .append(shortToQString(data_length))
                    .append(buffer,data_length);
            _socket->mySendingData(send_message);
            _send_size += data_length;
            qDebug()<<"_send_size: "<<_send_size;
            emit sending(_file_size, _send_size);
        }while (data_length > 0);
        _file.close();
        emit sendSuccess();*/
        QByteArray send_message;
        quint32 data_length = 0;
        quint32 id = 0;
        do{
            //这玩意是用来防止黏包的笨办法，linux系统建议改为20
            thread()->msleep(10);

            send_message.clear();
            char buffer[1024] = {0};
            data_length = _file.read(buffer, sizeof(buffer));
            send_message = packaging(++id, data_length, buffer);
            _send_size += data_length;
            _socket->mySendingData(send_message);
            qDebug()<<"_send_size: "<<_send_size;
            emit sending(_file_size, _send_size);
        }while (data_length > 0);
        _file.close();
        emit sendSuccess();
    }
}

/**
 * 函数名称：toSendFile()
 * 描述：通过网络链接文件接收者，为发送文件做准备
 * 参数：void
 * 返回值:void
 * 做成时间：2023.08.26
 * 作者：刘文景
 */
void FileSendBackground::toSendFile(){
    _socket = new MyTcpSocket;
    _socket->moveToThread(this->thread());
    _socket->myConnectToHost(_receiver_ip, _port);
    connect(_socket, SIGNAL(connected()),
            this, SLOT(connected_slot()));
}

/**
 * 函数名称：packaging()
 * 描述：通过网络链接文件接收者，为发送文件做准备
 * 参数：
 * 1.id : quint32 输入，表示该包的编号
 * 2.length : quint32 ，输入， 表示该包内容大小
 * 3.buffer ： char* , 输入，包的内容
 * 返回值:
 * QByteArray 包
 * 做成时间：2023.08.29
 * 作者：刘文景
 */
QByteArray FileSendBackground::packaging(
        quint32 id, quint32 length, char* buffer){
    QByteArray package;
    QByteArray arrid, arrlength;
    for(int i = 0;i < 4;i++){
        arrid.append(char((id<<(i*8))>>24));
        arrlength.append(char((length<<(i*8))>>24));
    }
    quint32 tag = 1024 - length;
    package.append(arrid)
            .append(arrlength)
            .append(buffer,length)
            .append(tag,'\0');
    return package;
}
