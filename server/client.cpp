#include "client.h"

Client::Client(QTcpSocket *socket, QHash<QString, Client *> &clientMap, Database &db, QReadWriteLock &lock) : clientMap(clientMap), db(db), lock(lock), socket(socket) {}

Client::~Client() {
    qDebug() << "destructing client" << this;
}

void Client::send(char *sendArr) {
    char *dat = new char[1408];
    memcpy(dat, sendArr, 1408);
    emit writeData(dat, socket);
}

int Client::makeFriendPacket(FriendlyBufferHelper *helper, QVector<QString> &friends, int start) {
    // 数据库获取好友列表
    helper->skip(2);
    int i = start, j = start;
    for (; helper->size() <= 1300 && i < friends.size(); i++) {
        QString fName = friends[i];
        QByteArray tmpArray = fName.toUtf8();
        // 用户名
        helper->write((quint8) tmpArray.length());
        helper->writeArray(tmpArray.data(), tmpArray.length());
        // ip
        if (fName.startsWith('_') || !clientMap.contains(fName)) {
            helper->write((quint32) 0);
        } else {
            helper->write(clientMap[fName]->socket->peerAddress().toIPv4Address());
        }
        // 是否在线
        if (fName.startsWith('_') || clientMap.contains(fName)) {
            helper->write((quint8) 1);
        } else {
            helper->write((quint8) 0);
        }
    }
    helper->reset();
    helper->write(C_FRIEND_LIST);
    helper->write((quint8) (i - j));
    if (i == friends.size()) {
        return -1;
    }
    return i;
}
/*
void Client::sendFriendPacket(QString name, FriendlyBufferHelper *helper) {
    QVector<QString> friends = db.selectFriend(name, lock);
    int i = 0;
    while (i != -1) {
        i = makeFriendPacket(helper, friends, i);
        clientMap[name]->send(helper->getBuffer());
        helper->reset();
    }
    helper->write(C_FRIEND_LIST);
    helper->write((quint8) 0);
    clientMap[name]->send(helper->getBuffer());
}*/


void Client::onReadyRead() {
    char recvArr[1408], sendArr[1408];
    while (true) {
        int len = socket->read(recvArr, 1408);
        if (len == 0) {
            return;
        }
        if (len == 1408) {
//            qDebug() << "receive" << QByteArray(recvArr, 1408) << "from" << name << "current thread:" << QThread::currentThread();
            FriendlyBufferHelper readHelper;
            readHelper.setBuffer(recvArr);
            quint8 code;
            readHelper.read(&code);
            if (code != S_LOGIN && code != S_REGISTER && name == "") {
                qDebug() << "invalid operation";
                emit close(socket);
                break;
            }

            quint8 nameLen;
            readHelper.read(&nameLen);
            if (nameLen > 30) {
                break;
            }

            char nameBuf[256] = {0}, fileNameBuf[256] = {0};
            QByteArray selfNameQArr = name.toUtf8();
            char *selfNameArr = selfNameQArr.data();
            quint8 selfNameLen = selfNameQArr.length();
            FriendlyBufferHelper sendHelper;
            readHelper.readArray(nameBuf, nameLen);
            QString targetName(nameBuf);
            sendHelper.setBuffer(sendArr);

            if (code == S_ACCEPT_FILE) {  // 接受文件请求
                qDebug() << name << "accept file";
                quint16 port;
                readHelper.read(&port);
                if (clientMap.contains(targetName)) {
                    // 对面在线就直接转发接受
                    sendHelper.write(S_ACCEPT_FILE);
                    sendHelper.write(selfNameLen);
                    sendHelper.writeArray(selfNameArr, selfNameLen);
                    sendHelper.write(port);
                    clientMap[targetName]->send(sendArr);
                } else {
                    // 否则返回一个拒绝给发送方
                    sendHelper.write(C_REJECT_FILE);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    send(sendArr);
                }
            } else if (code == S_REJECT_FILE) {  // 拒绝文件请求
                qDebug() << name << "reject file";
                if (clientMap.contains(targetName)) {
                    sendHelper.write(C_REJECT_FILE);
                    sendHelper.write(selfNameLen);
                    sendHelper.writeArray(selfNameArr, selfNameLen);
                    clientMap[targetName]->send(sendArr);
                }
            } else if (code == S_MAKE_GROUP) {  // 创建群聊
                if (nameBuf[0] == '_' && db.selectUser(targetName, lock).name == "" && db.selectUser(targetName.remove(0, 1), lock).name == "") {
                    db.insertUser("_" + targetName, "", lock);
                    sendHelper.write(C_MAKE_GROUP_SUCCESS);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                } else {
                    sendHelper.write(C_ERR_GROUP_CREATE);
                }
                send(sendArr);
            } else if (code == S_ADD_FRIEND) {  // 添加好友或群聊
                User user = db.selectUser(targetName, lock);
                if (user.name != "") {
                    db.insertFriend(name, targetName, lock);
                    sendHelper.write(C_ADD_FRIEND_SUCCESS);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    if (clientMap.contains(targetName)) {
                        sendHelper.write(clientMap[targetName]->socket->peerAddress().toIPv4Address());
                        sendHelper.write((quint8) 1);
                    } else {
                        sendHelper.write((quint32) 0);
                        if (targetName[0] == '_') {
                            sendHelper.write((quint8) 1);
                        } else {
                            sendHelper.write((quint8) 0);
                        }
                    }
                    send(sendArr);
                    sendHelper.reset();
                    if (clientMap.contains(targetName)) {
                        sendHelper.write(C_BEING_ADDED);
                        sendHelper.write(selfNameLen);
                        sendHelper.writeArray(selfNameArr, selfNameLen);
                        sendHelper.write(socket->peerAddress().toIPv4Address());
                        sendHelper.write((quint8) 1);
                        clientMap[targetName]->send(sendArr);
                    }
                } else {
                    sendHelper.write(C_ERR_FRIEND_NOT_EXIST);
                    send(sendArr);
                }
            } else if (code == S_DELETE_FRIEND) {  // 删除好友或群聊
                User user = db.selectUser(targetName, lock);
                // 检查有没有叫nameBuf的用户
                if (user.name != "") {
                    // 数据库中删除好友
                    db.deleteFriend(name, user.name, lock);
                    sendHelper.write(C_DELETE_FRIEND_SUCCESS);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    send(sendArr);
                    sendHelper.reset();
                    // 告知被删除的人（如果他在线的话）
                    if (clientMap.contains(user.name)) {
                        sendHelper.write(C_BEING_DELETED);
                        sendHelper.write(selfNameLen);
                        sendHelper.writeArray(selfNameArr, selfNameLen);
                        clientMap[targetName]->send(sendArr);
                    }
                    if (nameBuf[0] == '_' && db.selectFriend(targetName, lock).size() == 0) {
                        db.deleteUser(targetName, lock);
                        db.deleteGroupHistory(targetName, lock);
                    }
                } else {
                    sendHelper.write(C_ERR_FRIEND_NOT_EXIST);
                    send(sendArr);
                }
            }
            else if (code == S_SEARCH_RECORD || code == S_SEARCH_RECORD_WITH_TIME) {  // 查询聊天记录
                QVector<History> history;
                if (code == S_SEARCH_RECORD) {
                    // 开始查询聊天记录
                    if (nameBuf[0] == '_') {
                        history = db.selectLatestGroupHistory(targetName, lock);
                    } else {
                        history = db.selectLatestHistory(name, targetName, lock);
                    }
                } else {
                    // 开始指定时间段的查询聊天记录
                    qint64 startTime, endTime;
                    readHelper.read(&startTime);
                    readHelper.read(&endTime);
                    if (nameBuf[0] == '_') {
                        history = db.selectGroupHistory(targetName, startTime, endTime, lock);
                    } else {
                        history = db.selectHistory(name, targetName, startTime, endTime, lock);
                    }
                }
                for (History dat : history) {
                    sendHelper.write(C_RECORD);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    QByteArray tmpArr = dat.from.toUtf8();
                    sendHelper.write((quint8) tmpArr.length());
                    sendHelper.writeArray(tmpArr.data(), tmpArr.length());
                    QByteArray tmpArr1 = dat.msg.toUtf8();
                    sendHelper.write((qint16) tmpArr1.length());
                    sendHelper.writeArray(tmpArr1.data(), tmpArr1.length());
                    sendHelper.write(dat.fontSize);
                    sendHelper.write(dat.style);
                    sendHelper.write(dat.color);
                    sendHelper.write(dat.timestamp);
                    send(sendArr);
                    sendHelper.reset();
                    //thread()->msleep(50);   // 半包的暂定方案
                }
                sendHelper.write(C_RECORD);
                sendHelper.write(nameLen);
                sendHelper.writeArray(nameBuf, nameLen);
                sendHelper.write((quint8) 0);
                send(sendArr);
            } else if (code == S_REGISTER || code == S_LOGIN) {
                char pwdMd5Buf[33] = {0};
                readHelper.readArray(pwdMd5Buf, 32);
                if (code == S_REGISTER) {  // 注册
                    // 检查有无重名
                    User user = db.selectUser(targetName, lock);
                    if (nameBuf[0] == '_' || user.name != "") {
                        sendArr[0] = C_ERR_REGISTER_NAME;
                    } else {
                        // 把nameBuf和md5写入数据库
                        qDebug() << targetName << "register successfully";
                        db.insertUser(targetName, QString(pwdMd5Buf), lock);
                        sendArr[0] = C_REGISTER_SUCCESS;
                    }
                    send(sendArr);
                    emit close(socket);
                } else {   // 登录
                    if (clientMap.contains(targetName)) {
                        sendHelper.write(C_ERR_LOGIN_ALERADY_IN);
                        send(sendArr);
                        emit close(socket);
                    } else {
                        // 数据库校验密码和用户名是否正确
                        User user = db.selectUser(targetName, lock);
                        if (nameBuf[0] == '_' || user.name == "" || user.pwdHash != QString(pwdMd5Buf)) {
                            qDebug() << "wrong pwd or name";
                            sendHelper.write(C_ERR_LOGIN_NAME_PWD);
                            send(sendArr);
                            emit close(socket);
                        } else {
                            // 设置自己的名字
                            name = targetName;
                            clientMap[name] = this;
                            qDebug() << name << "is online";
                            sendHelper.write(C_LOGIN_SUCCESS);
                            send(sendArr);
                            sendHelper.reset();
                            // 获取自己的好友列表
                            QVector<QString> friends = db.selectFriend(name, lock);
                            for (QString fName : friends) {
                                // 如果好友在线
                                if (clientMap.contains(fName)) {
                                    // 告知好友自己上线
                                    sendHelper.write(C_FRIEND_ONLINE);
                                    sendHelper.write(nameLen);
                                    sendHelper.writeArray(nameBuf, nameLen);
                                    sendHelper.write(socket->peerAddress().toIPv4Address());
                                    sendHelper.write((quint8) 1);
                                    clientMap[fName]->send(sendArr);
                                    sendHelper.reset();
                                }
                            }
                        }
                    }
                }
            } else if (code == S_REQUEST_FILE) {  // 文件描述
                quint8 fileNameLen;
                qint64 fileSize;
                if (clientMap.contains(targetName)) {
                    readHelper.read(&fileNameLen);
                    readHelper.readArray(fileNameBuf, fileNameLen);
                    readHelper.read(&fileSize);
                    qDebug() << name << "request file transmission to" << targetName << "filename" << fileNameBuf << "size" << fileSize;
                    sendHelper.write(C_REQUEST_FILE);
                    sendHelper.write(selfNameLen);
                    sendHelper.writeArray(selfNameArr, selfNameLen);
                    sendHelper.write(fileNameLen);
                    sendHelper.writeArray(fileNameBuf, fileNameLen);
                    sendHelper.write(fileSize);
                    clientMap[targetName]->send(sendArr);
                } else {
                    qDebug() << name << "request file transmission to" << targetName << "but he/she is not online";
                    sendHelper.write(C_REJECT_FILE);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    QThread::msleep(10);
                    send(sendArr);
                }
            } else if (code == S_MESSAGE) {  // 消息
                qint16 msgLen;
                QVector<QString> friends = db.selectFriend(name, lock);
                qDebug() << targetName;
                if (friends.contains(targetName)) {
                    quint8 fontSize, style;
                    qint32 color;
                    char tmp[1335] = {0};
                    readHelper.read(&msgLen);
                    if (msgLen <= 1335) {
                        sendHelper.write(C_MESSAGE_SUCCESS);
                        sendHelper.write(nameLen);
                        sendHelper.writeArray(nameBuf, nameLen);
                        send(sendArr);
                        sendHelper.reset();
                        readHelper.readArray(tmp, msgLen);
                        readHelper.read(&fontSize);
                        readHelper.read(&style);
                        readHelper.read(&color);
                        qDebug() << name << "send" << QString(tmp) << "to" << targetName << "length" << msgLen;
                        // 把聊天记录存进数据库
                        // 如果字号是0，则为特殊消息
                        if (fontSize == 0) {
                            // 如果对方不在线，则存储特殊消息
                            if (!clientMap.contains(targetName)) {
                                db.insertHistory(selfNameArr, targetName, QString(tmp), 14, style, color, QDateTime::currentMSecsSinceEpoch(), lock);
                            }
                        } else {
                            db.insertHistory(selfNameArr, targetName, QString(tmp), fontSize, style, color, QDateTime::currentMSecsSinceEpoch(), lock);
                        }

                        if (nameBuf[0] == '_') {
                            // 查找数据库，看看群里都有谁
                            QVector<QString> sameGroup = db.selectFriend(targetName, lock);
                            sendHelper.write(C_MESSAGE);
                            sendHelper.write(selfNameLen);
                            sendHelper.writeArray(selfNameArr, selfNameLen);
                            sendHelper.write(msgLen);
                            sendHelper.writeArray(tmp, msgLen);
                            sendHelper.write(nameLen);
                            sendHelper.writeArray(nameBuf, nameLen);
                            sendHelper.write(fontSize);
                            sendHelper.write(style);
                            sendHelper.write(color);
                            for (QString fName : sameGroup) {
                                // 如果在线
                                if (fName != name && clientMap.contains(fName)) {
                                    clientMap[fName]->send(sendArr);
                                }
                            }
                        } else if (clientMap.contains(targetName)) {
                            sendHelper.write(C_MESSAGE);
                            sendHelper.write(selfNameLen);
                            sendHelper.writeArray(selfNameArr, selfNameLen);
                            sendHelper.write(msgLen);
                            sendHelper.writeArray(tmp, msgLen);
                            sendHelper.write((quint8) 0);
                            sendHelper.write(fontSize);
                            sendHelper.write(style);
                            sendHelper.write(color);
                            clientMap[targetName]->send(sendArr);
                        }
                    } else {
                        sendHelper.write(C_ERR_MESSAGE);
                        sendHelper.write(nameLen);
                        sendHelper.writeArray(nameBuf, nameLen);
                        send(sendArr);
                    }
                } else {
                    sendHelper.write(C_ERR_MESSAGE);
                    sendHelper.write(nameLen);
                    sendHelper.writeArray(nameBuf, nameLen);
                    send(sendArr);
                }
            } else if (code == S_REQUEST_FRIEND) {  // 请求好友列表
                qDebug() << name << "requests friend list";
                QVector<QString> friends = db.selectFriend(name, lock);
                int i = 0;
                while (i != -1) {
                    i = makeFriendPacket(&sendHelper, friends, i);
                    send(sendArr);
                    sendHelper.reset();
                }
                sendHelper.write(C_FRIEND_LIST);
                sendHelper.write((quint8) 0);
                send(sendArr);
            }
        }
    }
}

void Client::onDisconnected() {
    qDebug() << "connection closed. client:" << this << "name:" << name;
    if (clientMap.contains(name)) {
        char sendArr[1408];
        FriendlyBufferHelper sendHelper;
        sendHelper.setBuffer(sendArr);
        QVector<QString> friends = db.selectFriend(name, lock);
        for (QString fName : friends) {
            if (clientMap.contains(fName)) {
                sendHelper.write(C_FRIEND_OFFLINE);
                sendHelper.write((quint8) name.toUtf8().length());
                sendHelper.writeArray(name.toUtf8().data(), name.toUtf8().length());
                clientMap[fName]->send(sendArr);
                sendHelper.reset();
            }
        }
        clientMap.remove(name);
    }
    disconnect(socket);
    socket->deleteLater();
    emit offline(this);
}
