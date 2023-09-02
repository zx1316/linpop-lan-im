#include "requesttoserver.h"
#include "friendlybufferhelper.h"
#include "user.h"
#include <QByteArray>
#include <QDate>
#include <QCryptographicHash>
#include <QHostAddress>

inline QString IPV4IntegerToString(quint32 ip) {
    if(ip==0){
        return "";
    }
    return QString("%1.%2.%3.%4")
            .arg((ip >> 24) & 0xFF)
            .arg((ip >> 16) & 0xFF)
            .arg((ip >> 8) & 0xFF)
            .arg(ip & 0xFF);
}

//void RequestToServer::socketConnect(QString ip, quint16 port) {
//    m_tcp.connectToHost(ip,port);
//}

RequestToServer::RequestToServer(QObject *parent) : QObject(parent)
{
    connect(&m_tcp, &QTcpSocket::connected, this, [=]()
    {
        qDebug()<<"Connected";
        emit connectedSignal();
    });
    connect(&m_tcp, &QTcpSocket::readyRead, this, [=](){
        while (true) {
            if (remaining != 0) {
                qDebug()<<"get message from server 半包修复生效中";
                m_tcp.read(recvArr + 1408 - remaining, remaining);
                QByteArray arr(recvArr, 1408);
                quint8 x = arr[0];
                qDebug()<< "x : " <<x;
                handleResponse(x, arr);
                remaining = 0;
            }
            int len = m_tcp.read(recvArr, 1408);
            qDebug() << "len:" <<len;
            if(len <= 0)
            {
                return;
            }
            else if (len == 1408)
            {
                qDebug()<<"get message from server 整包";
                QByteArray arr(recvArr, 1408);
                quint8 x = arr[0];
                qDebug()<< "x : " <<x;
                handleResponse(x, arr);
            }
            else
            {
                qDebug()<<"检测到半包";
                remaining = 1408 - len;
                return;
            }
        }
    });

    connect(&m_tcp, &QTcpSocket::disconnected, this,[=]{
        emit disconnectedSignal();
    });//试图解决delete就爆炸的问题  后期考虑：内存泄露

    connect(&m_tcp, &QTcpSocket::stateChanged, this, [=]{
        qDebug() << m_tcp.state();
        if (m_tcp.state() == QTcpSocket::UnconnectedState) {
            if (loginOrRegister == 1) {
                emit loginInSignal(3);
            } else if (loginOrRegister == 2) {
                emit registrationSignal(-1);
            }
            loginOrRegister = 0;
        }
    });
}

void RequestToServer::handleResponse(qint8 x,QByteArray& arr){
    switch (x)
    {
        case 0: //接收方接受文件传输
        receiveAgreeSendFileFromOther(arr);
        break;

        case 1: //拒绝文件传输
        receiveRefuseSendFileFromOther(arr);
        break;

        case 2: //注册成功
        loginOrRegister = 0;
        emit registrationSignal(1);
        break;

        case 3: //接收好友列表
        receiveFriendStateUpdate(arr);
        break;

        case 4: //接收消息记录
        getChatRecord(arr);
        break;

        case 5: //登录成功
        loginOrRegister = 0;
        emit loginInSignal(0);
        break;

        case 6: //聊天消息发送成功
        messageSendSuccessfully(arr);
        break;

        case 7: //添加好友/群聊成功
        addFriendSuccessfully(arr);
        break;

        case 8: //删除好友/群聊成功
        deleteFriendSuccessfully(arr);
        break;

        case 9: //接收文件描述
        informationOfReceiveFile(arr);
        break;

        case 10:    //创建群聊成功
        createGroupSuccessfully(arr);
        break;

        case 11:
        emit getMessage(arr);
        break;

        case 12:    //注册重名
        loginOrRegister = 0;
        emit registrationSignal(0);
        break;

        case 13:    //登录用户名和密码错误
        loginOrRegister = 0;
        emit loginInSignal(1);

        break;

        case 14:    //登录时已经在线
        loginOrRegister = 0;
        emit loginInSignal(2);

        break;

        case 15:    //创建群聊失败，该群已被创建
        emit createGroupFeedbackSignal(false,"");
        break;

        case 16:    //已经和该用户/群成为好友

        break;

        case 17:    //用户/群聊不存在
        emit addFriendFeedbackSignal(0,Friend(false,"",""));
        break;

        case 18:    //聊天信息发送失败
        requestSendChatFail(arr);
        break;

        case 19:    //好友上线
        friendOnline(arr);
        break;

        case 20:    //好友下线
        friendOffline(arr);
        break;

        case 21:    //被添加好友信息
        addedAsFriend(arr);
        break;

        case 22:    //被删除好友信息
        deletedByFriend(arr);
        break;
    }
}
//MD5加密+Username(salt)
QString passwordEncipher(QString username, QString password);
//将style转化为board, italics, underline
bool handleStyle(int style, int &board, int& italics, int& underline);
//将int32的color转换为QColor
QColor handleColor(int32_t color);

/* 0(uint8) name_len(uint8) sender(int8[]) port(uint16)
 * 我方接受文件传输
*/
void RequestToServer::requestAgreeSendFileToMe(User u, quint16 port)
{
    qDebug() << "requestRefuseSendFileToMe()";
    char buf[1408];
    FriendlyBufferHelper writeHelper;
    writeHelper.setBuffer(buf);

    quint8 x = 0;
    quint8 name_len = u.username.toUtf8().length();
    QString senderName = u.username;

    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(senderName.toUtf8().data(), senderName.toUtf8().length());
    writeHelper.write(port);
    qDebug() << x << name_len << senderName << port;
    m_tcp.write(buf, 1408);
}

/* 1(uint8) name_len(uint8) sender(int8[])
 * 我方拒绝文件传输
*/
void RequestToServer::requestRefuseSendFileToMe(User u)
{
    char buf[1408];
    FriendlyBufferHelper writeHelper;
    writeHelper.setBuffer(buf);

    quint8 x = 1;
    quint8 name_len = u.username.toUtf8().length();
    QString senderName = u.username;
    qDebug() << x << name_len << senderName;

    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(senderName.toUtf8().data(), senderName.toUtf8().length());

    m_tcp.write(buf, 1408);
}

//2(uint8) name_len(uint8) group_name(int8[] 要有前导下划线)  创建群聊
void RequestToServer::requestCreateGroup(QString group_name)
{
    qDebug()<<"requestCreateGroup(QString group_name, QVector<User> users)";
    FriendlyBufferHelper writeHelper;
//    char *buf = new char[1024];
    char buf[1408];
    writeHelper.setBuffer(buf);
    quint8 x = 2;
    quint8 name_len = group_name.toUtf8().length() + 1;
    QString group = "_" + group_name;
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(group.toUtf8().data(),group.toUtf8().length());
    qDebug()<< name_len << group ;
    m_tcp.write(buf, 1408);
}

//3(uint8) name_len(uint8) target(int8[]) 添加好友/群聊
void RequestToServer::requestAddFriend(User u)
{
    qDebug()<< "requestAddFriend(User u)";
    FriendlyBufferHelper writeHelper;
//    char *buf = new char[1024];
    char buf[1408];
    writeHelper.setBuffer(buf);

    quint8 x = 3;
    quint8 name_len;
    QString target;
    if(u.type == true)
    {
        name_len = u.username.toUtf8().length();
        target = u.username;
    }
    else
    {
        name_len = u.username.toUtf8().length() + 1;
        target = "_" + u.username;
    }
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(target.toUtf8().data(),target.toUtf8().length());
//    QByteArray arr(buf);
//    QDataStream stream(&arr, QIODevice::WriteOnly);
    //stream << x << name_len << target;
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << target;
}

//4(uint8) name_len(uint8) target(int8[]) 删除好友/群聊
void RequestToServer::requestDeleteFriend(User u)
{
    qDebug()<< "requestDeleteFriend(User u)";
    FriendlyBufferHelper writeHelper;
//    char *buf = new char[1024];
    char buf[1408];
    writeHelper.setBuffer(buf);

    quint8 x = 4;
    quint8 name_len;
    QString target;
    if(u.type == true)
    {
        name_len = u.username.toUtf8().length();
        target = u.username;
    }
    else
    {
        name_len = u.username.toUtf8().length() + 1;
        target = "_" + u.username;
    }
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(target.toUtf8().data(),target.toUtf8().length());
//    QByteArray arr(buf);
//    QDataStream stream(&arr, QIODevice::WriteOnly);
    //stream << x << name_len << target;
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << target;
}

//5(uint8) name_len(uint8) target(int8[]) 查询最近消息记录
void RequestToServer::requestChatRecord(User u)
{
    qDebug() << "requestChatRecord(User u)";
    FriendlyBufferHelper writeHelper;
//    char *buf = new char[1024];
    char buf[1408];
    writeHelper.setBuffer(buf);

    quint8 x = 5;
    quint8 name_len;
    QString target;
    if(u.type == true)
    {
        name_len = u.username.toUtf8().length();
        target = u.username;
    }
    else
    {
        name_len = u.username.toUtf8().length() + 1;
        target = "_" + u.username;
    }
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(target.toUtf8().data(),target.toUtf8().length());
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << target;
}

//将QDate转换为int64
int64_t convertQDateToInt64(const QDate& date)
{
    QDateTime dateTime(date, QTime(0,0));
    int64_t milliseconds = dateTime.toMSecsSinceEpoch();
    return milliseconds;
}

//6(uint8) name_len(uint8) target(int8[]) start_timestamp(int64) end_timestamp(int64) 查询指定时间段消息记录
void RequestToServer::requestChatRecord(User u, QDate start, QDate end)
{
    qDebug()<< "requestChatRecord(User u, QDate start, QDate end)";
    FriendlyBufferHelper writeHelper;
    char buf[1408];
    writeHelper.setBuffer(buf);
    quint8 x = 6;
    quint8 name_len;
    QString target;
    int64_t _start;
    int64_t _end;
    if(u.type == true)
    {
        name_len = u.username.toUtf8().length();
        target = u.username;
    }
    else
    {
        name_len = u.username.toUtf8().length() + 1;
        target = "_" + u.username;
    }
    _start = convertQDateToInt64(start);
    _end = convertQDateToInt64(end.addDays(1));
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(target.toUtf8().data(), target.toUtf8().length());
    writeHelper.write(_start);
    writeHelper.write(_end);
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << target << _start <<_end;
}


//6(uint8) 聊天消息发送成功
void RequestToServer::messageSendSuccessfully(QByteArray arr)
{
    qDebug()<< "messageSendSuccessfully()";
    quint8 x;
    quint8 name_len;
    QString name;
    FriendlyBufferHelper readHelper;
    char* recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100];
    readHelper.readArray(_name, name_len);
    name = QString::fromUtf8(_name, name_len);
    if(name.startsWith('_')){
        name.remove(0,1);
    }
    emit sendMessageFeedbackSignal(name,true);
}

//7(uint8) name_len(uint8) name(int8[]) pwdmd5(int8[32] 加盐后md5的16进制的字符串) 注册
void RequestToServer::requestRegister(QString username, QString password, QString ip, quint16 port)
{
    loginOrRegister = 2;
    m_tcp.connectToHost(ip, port);
    qDebug()<<"requestRegister(QString username, QString password)";
//    QByteArray arr;
    FriendlyBufferHelper writeHelper;
//    char *buf = new char[1408];
    char buf[1408];
    writeHelper.setBuffer(buf);

    quint8 x = 7;
    quint8 name_len = username.toUtf8().length();
    QString pwd = passwordEncipher(username.toUtf8(), password);

//    stream << x << name_len << name << pwd;
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(username.toUtf8().data(),name_len);
    writeHelper.writeArray(pwd.toUtf8().data(),32);
    qDebug()<< x << name_len << username << pwd;
//    QByteArray arr(buf);
//    QDataStream stream(&arr,QIODevice::WriteOnly);
    m_tcp.write(buf,1408);
}

QString passwordEncipher(QString username, QString password)
{
    QByteArray pwd = password.toUtf8() + username.toUtf8();
    QByteArray hash = QCryptographicHash::hash(pwd, QCryptographicHash::Md5);
    return QString(hash.toHex());
}

//8(uint8) name_len(uint8) name(int8[]) pwdmd5(int8[32] 加盐后md5的16进制的字符串) 登录
void RequestToServer::requestLogin(QString username, QString password, QString ip, quint16 port)
{
    loginOrRegister = 1;
    m_tcp.connectToHost(ip, port);
    qDebug()<< "requestLogin(QString username, QString password)";
    FriendlyBufferHelper writeHelper;
    char buf[1408];
    writeHelper.setBuffer(buf);
    quint8 x = 8;
    quint8 name_len = username.toUtf8().length();
    QString pwd = passwordEncipher(username.toUtf8(), password);
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(username.toUtf8().data(),name_len);
    writeHelper.writeArray(pwd.toUtf8().data(),32);
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << username << pwd;
}

//8(uint8) name_len(uint8) name(int8[]) 删除好友/群聊成功
void RequestToServer::deleteFriendSuccessfully(QByteArray arr)
{
    qDebug() << "deleteFriendSuccessfully(arr)";
    quint8 x;
    quint8 name_len;
    QString name;
    FriendlyBufferHelper readHelper;
    char* recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100];
    readHelper.readArray(_name, name_len);
    name = QString::fromUtf8(_name, name_len);
    if(name.startsWith('_'))
    {
        name = name.remove(0,1);
    }
    emit deleteFriendFeedbackSignal(true,name);
}

//10请求好友列表
void RequestToServer::requestFriendList()
{
    quint8 x = 10;
    FriendlyBufferHelper writeHelper;
    char buf[1408];
    writeHelper.setBuffer(buf);;
    writeHelper.write(x);
    qDebug()<<"requestFriendList()";
    m_tcp.write(buf,1408);
}

//11(uint8) name_len(uint8) receiver(int8[]) msg_len(int16) msg(int8[])
//fontsize(uint8) style(uint8 倒数第3个bit是粗体，倒数第2个bit是斜体，倒数第1个bit是下划线) color(int32) 发送好友/群聊消息
void RequestToServer::requestChat(User u, QString message, int fontsize, int style, QColor color)
{
    qDebug()<< "requestChat(User u, QString message, int fontsize, int style, QColor color)";
    FriendlyBufferHelper writeHelper;
//    char* buf = new char[1024];
    char buf[1408];
    writeHelper.setBuffer(buf);
    quint8 x = 11;
    QString receiver = u.type?u.username:"_"+u.username;
    quint8 name_len = receiver.toUtf8().length();
    QString msg = message;
    qint16 msg_len = message.toUtf8().length();

    quint8 _fontsize = fontsize;
    quint8 _style = style;
    int32_t _color = color.rgb();
    writeHelper.write(x);
    writeHelper.write(name_len);
    writeHelper.writeArray(receiver.toUtf8().data(), receiver.toUtf8().length());
    writeHelper.write(msg_len);
    writeHelper.writeArray(msg.toUtf8().data(), msg.toUtf8().length());
    writeHelper.write(_fontsize);
    writeHelper.write(_style);
    writeHelper.write(_color);
//    QByteArray arr(buf);
//    QDataStream stream(&arr, QIODevice::WriteOnly);
//    stream << x << name_len << receiver << msg_len << msg << _fontsize << _style << _color;
    m_tcp.write(buf, 1408);
    qDebug()<< x << name_len << receiver << msg_len << msg << _fontsize << _style << _color;
}


//以下是接收服务器的信息

//0(uint8) name_len(uint8) receiver(int8[]) port(uint16)
// 接受方接受文件传输
// 返回对方端口
void RequestToServer::receiveAgreeSendFileFromOther(QByteArray arr)
{
    qDebug() << "receiveAgreeSendFileFromOther()";
    qDebug() << arr;

    quint8 x;
    quint8 name_len;
    QString receiverName;
    quint16 port;
    char*recvArr = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char name[100];
    readHelper.readArray(name, name_len);
    receiverName = QString::fromUtf8(name, name_len);
    readHelper.read(&port);
    qDebug() << x << name_len << receiverName << port;
    emit sendFileFeedbackSignal(true,receiverName,port);
}

//1(uint8) name_len(uint8) receiver(int8[])
// 接受方/发送方拒绝文件传输
void RequestToServer::receiveRefuseSendFileFromOther(QByteArray arr)
{
    qDebug() << "receiveRefuseSendFileFromOther";
//    qDebug() << arr;
    quint8 x;
    quint8 name_len;
    QString receiverName;
    char* recvArr = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char name[100];
    readHelper.readArray(name, name_len);
    receiverName = QString::fromUtf8(name, name_len);
    qDebug() << x << name_len << receiverName;
    emit sendFileFeedbackSignal(false,receiverName,0);
}



//4(uint8) name_len(uint8) target(int8[]) name_len1(uint8 多包消息，结束标志是该参数为0)
//name1(int8[]) msg_len1(int16) msg1(int8[])
//fontsize1(uint8) style1(uint8) color1(int32) timestamp1(int64)... 消息记录，一次只发一条记录
void RequestToServer::getChatRecord(QByteArray arr)
{
    qDebug()<< "getChatRecord()";
//    QVector<ChatRecord> vec;
//    QDataStream stream(&arr, QIODevice::ReadOnly);
    quint8 x;
    quint8 name_len;
    QString target;
    quint8 name_len1;
    QString name1;
    qint16 msg_len1;
    QString msg1;
    quint8 fontsize1;
    quint8 style1;
    int32_t color1;
    qint64 timestamp1;
//    stream >> x >> name_len >> target >> name_len1;
    char name[100];
    FriendlyBufferHelper readHelper;
    char *recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    readHelper.readArray(name, name_len);
    target = QString::fromUtf8(name, name_len);
    readHelper.read(&name_len1);


    qDebug() << x << name_len << target << "name_len1: " << name_len1;
    if(name_len1 == 0)
    {
        QVector<ChatRecord> em = qHash[target];
        qHash.remove(target);
        if(target.startsWith('_')){
            target = target.remove(0,1);
        }
//        vec.clear();
        emit chatRecordFeedbackSignal(target,em);
    }
    else
    {
//        stream >> name1 >> msg_len1 >> msg1 >> fontsize1 >> style1 >> color1 >> timestamp1;

        char name[100];
        readHelper.readArray(name, name_len1);
        name1 = QString::fromUtf8(name, name_len1);

        if(!qHash.contains(target))
        {
            qHash.insert(target, QVector<ChatRecord>());
        }

        readHelper.read(&msg_len1);
        char message[1336];
        readHelper.readArray(message, msg_len1);
        msg1 = QString::fromUtf8(message, msg_len1);
        readHelper.read(&fontsize1);
        readHelper.read(&style1);
        int board1, italics1, underline1;
        handleStyle(style1, board1, italics1, underline1);
        readHelper.read(&color1);
        readHelper.read(&timestamp1);

        QDateTime t = QDateTime::fromMSecsSinceEpoch(timestamp1);
        qHash[target].prepend(ChatRecord(name1,"", t, msg1,fontsize1,handleColor(color1), board1, italics1, underline1));
        qDebug()<< name1 << msg_len1 << msg1 << fontsize1 << style1 << color1 << timestamp1 ;
    }

}

bool handleStyle(int style, int& board, int& italics, int& underline)
{
    qDebug()<< "style:" << style;
    underline = style % 2;
    style /= 2;
    italics = style % 2;
    style /= 2;
    board = style;
    qDebug()<<board<<italics<<underline;
    return 1;
}

QColor handleColor(int32_t color)
{
    int red = (color >> 16) & 0xFF;
    int green = (color >> 8) & 0xFF;
    int blue = color & 0xFF;
    int alpha = (color >> 24) & 0xFF;
    QColor c(red, green, blue, alpha);
    qDebug()<<c;
    return c;
}

//10(uint8) name_len(uint8) name(int8[] 处理下划线) 创建群聊成功
void RequestToServer::createGroupSuccessfully(QByteArray arr)
{
    quint8 x;
    quint8 name_len;
    QString group_name;
    FriendlyBufferHelper readHelper;
    char *recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100] = {0};
    readHelper.readArray(_name, name_len);
    group_name = QString::fromUtf8(_name, name_len);
    group_name.remove(0,1);
    qDebug()<<"group_name: " << group_name;
    emit createGroupFeedbackSignal(true,group_name);
}

//11(uint8) name_len(uint8) real_sender(int8[]) msg_len(int16) msg(int8[])
//name_len(uint8 这个为0表明是真人而不是群) group_name(int8[]) fontsize(uint8)
//style(uint8 倒数第3个bit是粗体，倒数第2个bit是斜体，倒数第1个bit是下划线) color(int32) 接受的消息
void RequestToServer::getMessage(QByteArray arr)
{
    qDebug()<<"getMessage()";
    char *recvArr = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(recvArr);
    quint8 x;
    quint8 name_len;
    QString real_sender;
    qint16 msg_len;
    QString msg;
    quint8 type;
    QString group_name;
    quint8 fontsize;
    quint8 style;
    int32_t color1;
    readHelper.read(&x);
    readHelper.read(&name_len);
    char name[100];
    readHelper.readArray(name, name_len);
    real_sender = QString::fromUtf8(name,name_len);
    readHelper.read(&msg_len);
    char message[1336];
    readHelper.readArray(message, msg_len);
    msg = QString::fromUtf8(message, msg_len);
    readHelper.read(&type);
    char group[100];
    readHelper.readArray(group, type);
    group_name = QString::fromUtf8(group, type);
    if (group_name != "") {
        group_name = group_name.remove(0, 1);
    }else{
        group_name = real_sender;
    }
    readHelper.read(&fontsize);
    readHelper.read(&style);
    readHelper.read(&color1);
//    QDataStream stream(&arr, QIODevice::ReadOnly);
//    stream >> x >> name_len >> real_sender >> msg_len >> msg >> type >> group_name
//            >> fontsize >> style >> color1;
    qDebug()<< x << name_len <<real_sender <<msg_len << msg<< type << group_name << fontsize << style << color1;
    int board1, italics1, underline1;

    handleStyle(style, board1, italics1, underline1);
    emit newMessageSignal(group_name,real_sender, msg,handleColor(color1), board1, italics1, underline1,fontsize);
}

/*receiveRegister
 * 接受注册状态信号
 * 返回bool
 * 成功返回true，失败返回false
 */
bool RequestToServer::receiveRegister(qint8 state)
{
    qDebug()<<"receive register";
    qDebug()<< state;
    if(state == 2)//注册成功
    {
        return  true;
    }
    else if(state == 12)//重名
    {
        return  false;
    }
    qDebug()<<"Wrong Signal";
    return  false;
}

/*receiveLogin
 * 接受登录状态信号
 * 返回int::
 * 成功 - 0
 * 登录用户名或密码错误 - 1
 * 登录时已经在线 - 2
 */
int RequestToServer::receiveLogin(qint8 state,QByteArray arr)
{

    if(state == 3)//接受包:3(uint8) count(uint8)...      //登录成功包长大于1(?)
    {
        qDebug()<<"Login Success";
        receiveFriendStateUpdate(arr);//读取好友列表
        return 0;
    }
    else
    {
        //        QDataStream stream(&buf, QIODevice::ReadOnly);

        if(state == 13)//13(uint8) 登录用户名或密码错误

        {
            qDebug()<<"Login Wrong";
            return 1;//返回1
        }
        else if(state == 14) //14(uint8) 登录时已经在线
        {
            qDebug()<<"Login Already online";
            return 2;//返回2
        }
    }

    qDebug()<<"Wrong Signal";
    return  1;
}

//17(uint8) 用户/群聊不存在


/* receiveFriendStateUpdate
 * 解包返回Friend类列表
 */
//3(uint8) count(uint8) name_len1(uint8) name1(int8[]) ip(uint32) flag21(uint8 0-不在线 1-在线)...
//更新好友及在线情况（登录成功/添加好友或群聊成功/创建群聊/在线情况发生变化就会收到这个包）
void RequestToServer::receiveFriendStateUpdate(QByteArray in)
{
    qDebug() << "receiveFriendStateUpdate()";
    qDebug() << in;
    //QDataStream stream(&in, QIODevice::ReadOnly);
    char *recvArr = in.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(recvArr);

//    readHelper.read(&friendCount);
//    stream >> x >> friendCount;

//    if(friendCount == 0) {
//        qDebug() << "receiveFriendStateUpdate() end";
//        return friendList;
//    } else {
    quint8 x;
    quint8 friendCount;
    readHelper.read(&x);
    readHelper.read(&friendCount);
    qDebug()<< x <<friendCount;
    if(friendCount == 0) {
        qDebug() << "receiveFriendStateUpdate() end";
        qDebug()<<"send friend list";
        for (auto a : friendList) {
            qDebug() << a._name;
        }
        emit friendListFeedbackSignal(friendList);
        friendList.clear();
    }
    for (int i = 0; i < friendCount; i++) {
        char tmpName[32]={0};
        quint8 inNameLen;
        quint32 inIp;
        quint8 inIsOnline;
        quint8 inIsReal;
        qDebug()<<"read friend list";
        readHelper.read(&inNameLen);
        readHelper.readArray(tmpName, inNameLen);
        readHelper.read(&inIp);
        readHelper.read(&inIsOnline);
        QString realName(tmpName);
        qDebug()<< inNameLen << tmpName << inIp << inIsOnline;
        if(tmpName[0] == '_') { //群聊以_开头
            inIsReal = 0;
            realName = realName.remove(0, 1);
        } else {
            inIsReal = 1;
        }
        friendList.push_back(Friend(inIsOnline, realName, IPV4IntegerToString(inIp),inIsReal));
    }
}

//7(uint8) name_len(uint8) name(int8[]) ip1(uint32) flag(uint8 0-不在线 1-在线) 添加好友/群聊成功
void RequestToServer::addFriendSuccessfully(QByteArray arr)
{
    qDebug()<< "addFriendSuccessfully(arr)";
    quint8 x;
    quint8 name_len;
    QString name;
    quint32 ip1;
    quint8 flag;
    FriendlyBufferHelper readHelper;
    char* recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100];
    readHelper.readArray(_name, name_len);
    name = QString::fromUtf8(_name, name_len);
    bool type = name.startsWith('_');
    if(type){
        name = name.remove(0,1);
    }
    readHelper.read(&ip1);
    readHelper.read(&flag);
    QString ip = IPV4IntegerToString(ip1);
    //提取结束
    emit addFriendFeedbackSignal(true,Friend(flag,name,ip,!type));
}

//9(uint8) name_len(uint8) sender(int8[]) file_name_len(uint8) file_name(int8[]) file_size(int64) 接收对方发送的文件传输请求
void RequestToServer::informationOfReceiveFile(QByteArray arr)
{
    qDebug()<< "informationOfReceiveFile(arr)";
    quint8 x;
    quint8 name_len;
    QString sender;
    quint8 file_name_len;
    QString file_name;
    int64_t file_size;
    FriendlyBufferHelper readHelper;
    char* recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100];
    readHelper.readArray(_name, name_len);
    sender = QString::fromUtf8(_name, name_len);
    readHelper.read(&file_name_len);
    char _file[100];
    readHelper.readArray(_file, file_name_len);
    file_name = QString::fromUtf8(_file, file_name_len);
    readHelper.read(&file_size);
    //提取结束
    qDebug()<< x << name_len << sender << file_name_len << file_name << file_size;
    emit beRequestedSentFileSignal(sender,file_name,file_size);

}

//9(uint8) name_len(uint8) receiver(int8[]) file_name_len(uint8) file_name(int8[]) file_size(int64) 发送文件传输请求
void RequestToServer::requestSendFile(QString receiver, QString file_name, qint64 file_size)
{
    qDebug()<< "requestSendFile()";
    FriendlyBufferHelper writeHelper;
    char buf[1408];
    writeHelper.setBuffer(buf);
    quint8 x = 9;
    qDebug()<<receiver;
    writeHelper.write(x);
    writeHelper.write((quint8) receiver.toUtf8().length());
    writeHelper.writeArray(receiver.toUtf8().data(), receiver.toUtf8().length());
    writeHelper.write((quint8)file_name.toUtf8().length());
    writeHelper.writeArray(file_name.toUtf8().data(), file_name.toUtf8().length());
    writeHelper.write(file_size);
    m_tcp.write(buf,1408);
}

void requestSendChatFail(QByteArray arr);                      //18

// 18(uint8) name_len(uint8) name(int8[]) 聊天信息发送失败
void RequestToServer::requestSendChatFail(QByteArray arr)
{
    qDebug() << "requestSendChatFail()";
//    qDebug() << arr;

    char *recvArr = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(recvArr);
    quint8 x;
    quint8 name_len;
    QString senderName;

    readHelper.read(&x);
    readHelper.read(&name_len);
    char name[100];
    readHelper.readArray(name, name_len);
    senderName = QString::fromUtf8(name, name_len);

    qDebug() << x << name_len << senderName;
    emit sendMessageFeedbackSignal(name,false);
}

//20(uint8) name_len(uint8) name(int8[]) 好友下线
void RequestToServer::friendOffline(QByteArray arr)
{
    qDebug()<< "friendOffline(arr)";
    quint8 x;
    quint8 name_len;
    QString name;
    FriendlyBufferHelper readHelper;
    char* recvArr = arr.data();
    readHelper.setBuffer(recvArr);
    readHelper.read(&x);
    readHelper.read(&name_len);
    char _name[100];
    readHelper.readArray(_name, name_len);
    name = QString::fromUtf8(_name, name_len);
    //提取结束
//    qDebug() << x << name_len << name;
    emit friendDownSignal(name);
}

/*
 * 19好友上线
 * 返回值：上线好友整个Friend类信息
 */
void RequestToServer::friendOnline(QByteArray arr)
{

    char *buf = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(buf);
    quint8 name_len;
    quint8 x;
    QString onlineName;
    quint32 ip1;
    readHelper.read(&x);
    readHelper.read(&name_len);
    char name[100]={0};
    readHelper.readArray(name, name_len);
    onlineName = QString::fromUtf8(name, name_len);
    readHelper.read(&ip1);
//    qDebug()<<name_len<<name<<ip1;
    qDebug()<<onlineName;
    emit friendUpSignal(Friend(true,onlineName,IPV4IntegerToString(ip1),true));
}

/*
 * 21被添加为好友
 * 返回值：添加好友者整个Friend类信息
 */
void RequestToServer::addedAsFriend(QByteArray arr)
{
    char *buf = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(buf);
    quint8 name_len;
    QString onlineName;
    quint32 ip1;
    readHelper.read(&name_len);
    readHelper.read(&name_len);
    char name[100]={0};
    readHelper.readArray(name, name_len);
    onlineName = QString::fromUtf8(name, name_len);
    bool type = true;
    if(onlineName.startsWith('_')){
        type = false;
        onlineName = onlineName.remove(0,1);
    }
    readHelper.read(&ip1);
    qDebug()<<name_len<<name<<ip1;
    emit beAddedSignal(Friend(true,onlineName,IPV4IntegerToString(ip1),type));
}

/*
 * 22被删除好友
 * 返回值：删除好友者username (QString)
 */
void RequestToServer::deletedByFriend(QByteArray arr)
{
    char *buf = arr.data();
    FriendlyBufferHelper readHelper;
    readHelper.setBuffer(buf);
    quint8 name_len;
    QString username;
    readHelper.read(&name_len);
    readHelper.read(&name_len);
    char name[100]={0};
    readHelper.readArray(name, name_len);
    username = QString::fromUtf8(name, name_len);
    qDebug()<<username;
    emit beDeletedSignal(username);
}

QString RequestToServer::getLocalAddress() {
    return IPV4IntegerToString(m_tcp.localAddress().toIPv4Address());
}
