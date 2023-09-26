#include "client.h"
#include "clientrecvthread.h"
#include "clientsendthread.h"
#include <QTcpServer>
#include <QCryptographicHash>
#include <QDateTime>

Client::Client(QTcpSocket *socket, QHash<QString, Client *> &clientMap, QMultiHash<QString, QJsonObject> &imgJsonMap, Database &db, QMutex &clientMapLock) : clientMapLock(clientMapLock), clientMap(clientMap), imgJsonMap(imgJsonMap), db(db), socket(socket) {
    QTimer::singleShot(5000, this, [=] {
        if (name == "") {
            qDebug() << "auto disconnect client" << this;
            emit close(socket);
        }
    });
}

Client::~Client() {
    socket->deleteLater();
    delete[] recvArr;
    qDebug() << "destructing client" << this;
}

void Client::send(const QJsonObject &object) {
    QJsonDocument document(object);
    auto array = document.toJson(QJsonDocument::Compact);
    qint32 len = array.length();
    if (len > 4096) {
        array = qCompress(array);
        len = -array.length();
    }
    array.insert(0, reinterpret_cast<char *>(&len), 4);
    emit writeData(array, socket);
}

void Client::handleJson(const QJsonObject &jsonObject) {
    auto cmd = jsonObject["cmd"].toString();
    if (name == "" && cmd != "register" && cmd != "login" && cmd != "img") {
        return;
    }
    qDebug() << "handle" << cmd << "client" << this << "user" << name;
    QJsonObject objectReturn, objectToOthers;
    if (cmd == "register") {
        auto name1 = jsonObject["name"].toString();
        auto pwd = jsonObject["pwd"].toString();
        auto imgName = jsonObject["img"].toString();
        int result = 2;
        if (!(name1.isNull() || pwd.isNull() || imgName.isNull())) {
            bool flag = false;
            if (name1[0] != '_') {
                result = db.insertUser(name1, pwd, imgName);
                if (result == 0) {
                    objectReturn["cmd"] = cmd;
                    objectReturn["status"] = "success";
                } else if (result == 1) {
                    if (imgJsonMap.contains(imgName)) {
                        flag = true;
                    }
                    imgJsonMap.insert(imgName, jsonObject);
                    objectReturn["cmd"] = "request_img";
                    objectReturn["img"] = imgName;
                } else {
                    objectReturn["cmd"] = cmd;
                    objectReturn["status"] = "fail";
                }
            } else {
                objectReturn["cmd"] = cmd;
                objectReturn["status"] = "fail";
            }
            if (!flag) {
                send(objectReturn);
            }
        }
        if (result != 1) {
            emit close(socket);
        }
    } else if (cmd == "login") {
        objectReturn["cmd"] = cmd;
        auto name1 = jsonObject["name"].toString();
        auto pwd = jsonObject["pwd"].toString();
        if (name1.isNull() || pwd.isNull()) {
            return;
        }
        clientMapLock.lock();
        if (clientMap.contains(name1)) {
            objectReturn["status"] = "already";
            objectReturn["img"] = "";
            objectReturn["friends"] = QJsonArray();
            send(objectReturn);
            emit close(socket);
        } else {
            auto user = db.selectUser(name1);
            if (name1[0] == '_' || user.name == "" || user.pwd != pwd) {
                objectReturn["status"] = "unauthorized";
                objectReturn["img"] = "";
                objectReturn["friends"] = QJsonArray();
                send(objectReturn);
                emit close(socket);
            } else {
                clientMap[name1] = this;
                name = name1;
                objectReturn["status"] = "success";
                objectReturn["img"] = user.imgName;
                QJsonArray array;
                auto list1 = db.selectFriends(name);
                for (auto item : list1) {
                    QJsonObject obj;
                    obj["name"] = item.name;
                    obj["img"] = item.imgName;
                    obj["ip"] = clientMap.contains(item.name) ? clientMap[item.name]->socket->peerAddress().toString() : "";
                    obj["new_msg"] = item.isNewMsg;
                    array.append(obj);
                }
                objectReturn["friends"] = array;
                send(objectReturn);
                objectToOthers["cmd"] = "online";
                objectToOthers["name"] = name;
                objectToOthers["ip"] = socket->peerAddress().toString();
                for (auto item : list1) {
                    if (clientMap.contains(item.name)) {
                        clientMap[item.name]->send(objectToOthers);
                    }
                }
            }
        }
        clientMapLock.unlock();
    } else if (cmd == "group_member_list") {
        objectReturn["cmd"] = cmd;
        auto groupName = jsonObject["group_name"].toString();
        if (groupName.isNull()) {
            return;
        }
        objectReturn["group_name"] = groupName;
        QJsonArray array;
        auto list = db.selectFriends(groupName);
        for (auto item : list) {
            array.append(item.name);
        }
        objectReturn["members"] = array;
        send(objectReturn);
    } else if (cmd == "create_group") {
        auto groupName = jsonObject["group_name"].toString();
        auto imgName = jsonObject["img"].toString();
        if (groupName.isNull() || imgName.isNull() || !jsonObject["init"].isArray()) {
            return;
        }
        auto array = jsonObject["init"].toArray();
        for (auto item : array) {
            if (!item.isString()) {
                return;
            }
        }
        bool flag = false;
        if (groupName[0] == '_') {
            auto result = db.insertUser(groupName, "", imgName);
            if (result == 0) {
                objectReturn["cmd"] = cmd;
                objectReturn["status"] = "success";
                objectToOthers["cmd"] = "be_added";
                objectToOthers["name"] = groupName;
                objectToOthers["ip"] = "";
                objectToOthers["img"] = imgName;
                clientMapLock.lock();
                for (auto item : array) {
                    QString name1 = item.toString();
                    db.insertFriend(name1, groupName);
                    if (clientMap.contains(name1)) {
                        clientMap[name1]->send(objectToOthers);
                    }
                }
                clientMapLock.unlock();
                QDir dir(QCoreApplication::applicationDirPath() + "/group_files");
                dir.mkdir(groupName);
            } else if (result == 1) {
                if (imgJsonMap.contains(imgName)) {
                    flag = true;
                }
                imgJsonMap.insert(imgName, jsonObject);
                objectReturn["cmd"] = "request_img";
                objectReturn["img"] = imgName;
            } else {
                objectReturn["cmd"] = cmd;
                objectReturn["status"] = "fail";
            }
        } else {
            objectReturn["cmd"] = cmd;
            objectReturn["status"] = "fail";
        }
        if (!flag) {
            send(objectReturn);
        }
    } else if (cmd == "add") {
        auto name1 = jsonObject["name"].toString();
        if (name1.isNull()) {
            return;
        }
        objectReturn["cmd"] = cmd;
        objectReturn["name"] = name1;
        if (db.insertFriend(name, name1)) {
            clientMapLock.lock();
            objectReturn["status"] = "success";
            objectReturn["ip"] = clientMap.contains(name1) ? clientMap[name1]->socket->peerAddress().toString() : "";
            objectReturn["img"] = db.selectUser(name1).imgName;
            if (clientMap.contains(name1)) {
                objectToOthers["cmd"] = "be_added";
                objectToOthers["name"] = name;
                objectToOthers["ip"] = socket->peerAddress().toString();
                objectToOthers["img"] = db.selectUser(name).imgName;
                clientMap[name1]->send(objectToOthers);
            }
            clientMapLock.unlock();
        } else {
            objectReturn["status"] = "fail";
            objectReturn["ip"] = "";
            objectReturn["img"] = "";
        }
        send(objectReturn);
    } else if (cmd == "delete") {
        auto name1 = jsonObject["name"].toString();
        if (name1.isNull()) {
            return;
        }
        clientMapLock.lock();
        if (db.deleteFriend(name, name1) && clientMap.contains(name1)) {
            objectToOthers["cmd"] = "be_deleted";
            objectToOthers["name"] = name;
            clientMap[name1]->send(objectToOthers);
        }
        clientMapLock.unlock();
        // 删除群文件
        if (name1[0] == '_' && db.selectUser(name1).name == "") {
            QDir dir(QCoreApplication::applicationDirPath() + "/group_files/" + name1);
            dir.removeRecursively();
        }
    } else if (cmd == "send_msg") {
        auto receiver = jsonObject["receiver"].toString();
        auto type = jsonObject["type"].toString();
        auto msg = jsonObject["msg"].toString();
        if (receiver.isNull() || type.isNull() || msg.isNull()) {
            return;
        }
        if (!db.insertChatRecord(name, receiver, msg, type, QDateTime::currentMSecsSinceEpoch())) {
            if (imgJsonMap.contains(msg)) {
                imgJsonMap.insert(msg, jsonObject);
            } else {
                imgJsonMap.insert(msg, jsonObject);
                objectReturn["cmd"] = "request_img";
                objectReturn["img"] = msg;
                send(objectReturn);
            }
        } else {
            objectToOthers["cmd"] = "new_msg";
            objectToOthers["sender"] = name;
            objectToOthers["msg"] = msg;
            objectToOthers["type"] = type;
            clientMapLock.lock();
            if (receiver[0] == '_') {
                objectToOthers["inner_name"] = receiver;
                auto list = db.selectFriends(receiver);
                for (auto item : list) {
                    if (name != item.name && clientMap.contains(item.name)) {
                        clientMap[item.name]->send(objectToOthers);
                    }
                }
            } else {
                objectToOthers["inner_name"] = name;
                if (clientMap.contains(receiver)) {
                    clientMap[receiver]->send(objectToOthers);
                }
            }
            clientMapLock.unlock();
            objectReturn["cmd"] = "send_msg";
            objectReturn["receiver"] = receiver;
            objectReturn["status"] = "success";
            send(objectReturn);
        }
    } else if (cmd == "request_img") {
        auto imgName = jsonObject["img"].toString();
        if (imgName.isNull()) {
            return;
        }
        objectReturn["cmd"] = "img";
        objectReturn["img"] = imgName;
        objectReturn["base64"] = QString(db.selectImg(imgName).toBase64());
        send(objectReturn);
    } else if (cmd == "img") {
        auto imgName = jsonObject["img"].toString();
        auto base64 = jsonObject["base64"].toString();
        qDebug() << imgName;
        if (imgName.isNull() || base64.isNull()) {
            return;
        }
        auto array = QByteArray::fromBase64(base64.toLatin1());
        if (QCryptographicHash::hash(array, QCryptographicHash::Md5).toHex() == imgName.left(imgName.indexOf('.')) && imgJsonMap.contains(imgName)) {
            db.insertImg(imgName, array);
            for (auto item : imgJsonMap.values(imgName)) {
                handleJson(item);
            }
            imgJsonMap.remove(imgName);
        }
    } else if (cmd == "history") {
        bool ok1, ok2, ok3;
        auto name1 = jsonObject["name"].toString();
        auto limit = jsonObject["limit"].toString().toUShort(&ok1);
        auto start = jsonObject["start"].toString().toLongLong(&ok2);
        auto end = jsonObject["end"].toString().toLongLong(&ok3);
        if (name1.isNull() || !ok1 || !ok2 || !ok3) {
            return;
        }
        auto list = db.selectChatRecords(name, name1, start, end, limit);
        objectReturn["cmd"] = cmd;
        objectReturn["name"] = name1;
        QJsonArray array;
        for (auto item : list) {
            QJsonObject obj;
            obj["sender"] = item.sender;
            obj["msg"] = item.msg;
            obj["timestamp"] = QString::number(item.timestamp);
            obj["type"] = item.type;
            array.append(obj);
        }
        objectReturn["records"] = array;
        send(objectReturn);
    } else if (cmd == "request_file") {
        bool ok;
        auto receiver = jsonObject["receiver"].toString();
        auto fileName = jsonObject["file_name"].toString();
        auto sizeStr = jsonObject["size"].toString();
        sizeStr.toLongLong(&ok);
        if (receiver.isNull() || fileName.isNull() || !ok) {
            return;
        }
        clientMapLock.lock();
        if (clientMap.contains(receiver)) {
            objectToOthers["cmd"] = cmd;
            objectToOthers["sender"] = name;
            objectToOthers["file_name"] = fileName;
            objectToOthers["size"] = sizeStr;
            clientMap[receiver]->send(objectToOthers);
        } else {
            objectReturn["cmd"] = "response_file";
            objectReturn["receiver"] = receiver;
            objectReturn["port"] = "-1";
            send(objectReturn);
        }
        clientMapLock.unlock();
    } else if (cmd == "response_file") {
        bool ok;
        auto sender = jsonObject["sender"].toString();
        auto portStr = jsonObject["port"].toString();
        portStr.toUShort(&ok);
        if (sender.isNull() || !ok) {
            return;
        }
        clientMapLock.lock();
        if (clientMap.contains(sender)) {
            objectToOthers["cmd"] = cmd;
            objectToOthers["receiver"] = name;
            objectToOthers["port"] = portStr;
            clientMap[sender]->send(objectToOthers);
        }
        clientMapLock.unlock();
    } else if (cmd == "change_img") {
        auto imgName = jsonObject["img"].toString();
        if (imgName.isNull()) {
            return;
        }
        if (db.updateUserImg(name, imgName)) {
            objectToOthers["cmd"] = "img_changed";
            objectToOthers["name"] = name;
            objectToOthers["img"] = imgName;
            auto list = db.selectFriends(name);
            clientMapLock.lock();
            for (auto item : list) {
                if (clientMap.contains(item.name)) {
                    clientMap[item.name]->send(objectToOthers);
                }
            }
            clientMapLock.unlock();
        } else {
            if (imgJsonMap.contains(imgName)) {
                imgJsonMap.insert(imgName, jsonObject);
            } else {
                imgJsonMap.insert(imgName, jsonObject);
                objectReturn["cmd"] = "request_img";
                objectReturn["img"] = imgName;
                send(objectReturn);
            }
        }
    } else if (cmd == "file_list") {
        auto groupName = jsonObject["group_name"].toString();
        if (groupName.isNull()) {
            return;
        }
        auto list = db.selectFiles(groupName);
        QJsonArray array;
        objectReturn["cmd"] = cmd;
        objectReturn["group_name"] = groupName;
        for (auto item : list) {
            QJsonObject obj;
            obj["file_name"] = item.fileName;
            obj["uploader"] = item.uploader;
            obj["size"] = QString::number(item.size);
            obj["timestamp"] = QString::number(item.timestamp);
            array.append(obj);
        }
        objectReturn["files"] = array;
        send(objectReturn);
    } else if (cmd == "upload_file") {
        bool ok1, ok2;
        auto groupName = jsonObject["group_name"].toString();
        auto fileName = jsonObject["file_name"].toString();
        auto size = jsonObject["size"].toString().toLongLong(&ok1);
        auto port = jsonObject["port"].toString().toUShort(&ok2);
        if (groupName.isNull() || fileName.isNull() || !ok1 || !ok2) {
            return;
        }
        auto thread = new ClientRecvThread(QCoreApplication::applicationDirPath() + "/group_files/" + groupName + "/" + fileName, socket->peerAddress().toString(), port);
        auto timer = new QTimer;
        connect(thread, &ClientRecvThread::success, this, [=] {
            db.insertFile(groupName, fileName, name, size, QDateTime::currentMSecsSinceEpoch());
        });
        connect(thread, &ClientRecvThread::success, thread, &ClientRecvThread::quit);
        connect(thread, &ClientRecvThread::fail, thread, &ClientRecvThread::quit);
        connect(thread, SIGNAL(progress(qint8)), timer, SLOT(start()));
        connect(timer, &QTimer::timeout, thread, &ClientRecvThread::quit);
        connect(thread, &ClientRecvThread::finished, thread, &ClientRecvThread::deleteLater);
        connect(thread, &ClientRecvThread::finished, timer, &QTimer::deleteLater);
        timer->setInterval(30000);
        timer->start();
        thread->start();
    } else if (cmd == "download_file") {
        bool ok;
        auto groupName = jsonObject["group_name"].toString();
        auto fileName = jsonObject["file_name"].toString();
        auto port = jsonObject["port"].toString().toUShort(&ok);
        if (groupName.isNull() || fileName.isNull() || !ok) {
            return;
        }
        auto thread = new ClientSendThread(QCoreApplication::applicationDirPath() + "/group_files/" + groupName + "/" + fileName, socket->peerAddress().toString(), port);
        auto timer = new QTimer;
        connect(thread, &ClientSendThread::success, thread, &ClientSendThread::quit);
        connect(thread, &ClientSendThread::fail, thread, &ClientSendThread::quit);
        connect(thread, SIGNAL(progress(int8)), timer, SLOT(start()));
        connect(timer, &QTimer::timeout, thread, &ClientSendThread::quit);
        connect(thread, &ClientSendThread::finished, thread, &ClientSendThread::deleteLater);
        connect(thread, &ClientSendThread::finished, timer, &QTimer::deleteLater);
        timer->setInterval(30000);
        timer->start();
        thread->start();
    } else if (cmd == "delete_file") {
        auto groupName = jsonObject["group_name"].toString();
        auto fileName = jsonObject["file_name"].toString();
        if (db.selectFile(groupName, fileName).uploader == name) {
            db.deleteFile(groupName, fileName);
            QFile::remove(QCoreApplication::applicationDirPath() + "/group_files/" + groupName + "/" + fileName);
        }
    }
}

void Client::onReadyRead() {
    qint64 len;
    while ((len = socket->read(recvArr + recvLen, exceptLen - recvLen)) > 0) {
        recvLen += len;
        if (recvLen == exceptLen) {
            if (isReadyReadJson) {
                exceptLen = 4;
                // 开始处理
                QByteArray array;
                if (isCompressed) {
                    array = qUncompress(reinterpret_cast<unsigned char *>(recvArr), recvLen);
                } else {
                    array = QByteArray(recvArr, recvLen);
                }
                QJsonDocument document = QJsonDocument::fromJson(array);
                handleJson(document.object());
            } else {
                exceptLen = *reinterpret_cast<int *>(recvArr);
                if (exceptLen < 0) {
                    isCompressed = true;
                    exceptLen = -exceptLen;
                } else {
                    isCompressed = false;
                }
                if (exceptLen > 2 * 1024 * 1024) {
                    emit close(socket);
                    return;
                }
            }
            isReadyReadJson = !isReadyReadJson;
            recvLen = 0;
        }
    }
}

void Client::onDisconnected() {
    qDebug() << "connection closed. client" << this << "name" << name;
    clientMapLock.lock();
    if (clientMap.contains(name)) {
        db.updateOfflineTime(name, QDateTime::currentMSecsSinceEpoch());
        auto list = db.selectFriends(name);
        QJsonObject objectToOthers;
        objectToOthers["cmd"] = "offline";
        objectToOthers["name"] = name;
        for (auto item : list) {
            if (clientMap.contains(item.name)) {
                clientMap[item.name]->send(objectToOthers);
            }
        }
        clientMap.remove(name);
    }
    clientMapLock.unlock();
    emit clear(this);
}
