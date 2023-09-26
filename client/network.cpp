#include "network.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDateTime>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>

Network::Network(QObject *parent) : QObject{parent} {
    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, [=] {
        timer.stop();
        emit connectedSignal();
    });
    connect(&socket, &QTcpSocket::stateChanged, this, &Network::onStateChanged);
    connect(&socket, &QTcpSocket::readyRead, this, &Network::onReadyRead);
}

Network::~Network() {
    disconnect(&socket);
    disconnect(&timer);
    socket.disconnectFromHost();
    delete[] recvArr;
}

void Network::writeJson(const QJsonObject &obj) {
    QJsonDocument document(obj);
    auto array = document.toJson(QJsonDocument::Compact);
    qint32 len = array.length();
    if (len > 4096) {
        array = qCompress(array);
        len = -array.length();
    }
    array.insert(0, reinterpret_cast<char *>(&len), 4);
    socket.write(array);
}

void Network::requestRegister(const QString &name, const QString &pwdHash, const QString &imgName) {
    QJsonObject obj;
    obj["cmd"] = "register";
    obj["name"] = name;
    obj["pwd"] = pwdHash;
    obj["img"] = imgName;
    writeJson(obj);
}

void Network::requestLogin(const QString &name, const QString &pwdHash) {
    QJsonObject obj;
    obj["cmd"] = "login";
    obj["name"] = name;
    obj["pwd"] = pwdHash;
    writeJson(obj);
}

void Network::requestGroupMemberList(const QString &name) {
    QJsonObject obj;
    obj["cmd"] = "group_member_list";
    obj["group_name"] = name;
    writeJson(obj);
}

void Network::requestCreateGroup(const QString &groupName, const QString &imgName, const QList<QString> &memberList) {
    QJsonObject obj;
    obj["cmd"] = "create_group";
    obj["group_name"] = groupName;
    obj["img"] = imgName;
    QJsonArray arr;
    for (auto item : memberList) {
        arr.append(item);
    }
    obj["init"] = arr;
    writeJson(obj);
}

void Network::requestAddFriend(const QString &name) {
    QJsonObject obj;
    obj["cmd"] = "add";
    obj["name"] = name;
    writeJson(obj);
}

void Network::requestDeleteFriend(const QString &name) {
    QJsonObject obj;
    obj["cmd"] = "delete";
    obj["name"] = name;
    writeJson(obj);
}

void Network::requestSendMsg(const QString &receiver, const QString &msg, const QString &type) {
    QJsonObject obj;
    obj["cmd"] = "send_msg";
    obj["receiver"] = receiver;
    obj["msg"] = msg;
    obj["type"] = type;
    writeJson(obj);
    msgCache[receiver] = {"", msg, type, 0};
}

void Network::requestLatestHistory(const QString &name) {
    QJsonObject obj;
    obj["cmd"] = "history";
    obj["name"] = name;
    obj["limit"] = "50";
    obj["start"] = "0";
    obj["end"] = QString::number(QDateTime::currentMSecsSinceEpoch());
    writeJson(obj);
}

void Network::requestHistory(const QString &name, const QDate &start, const QDate &end) {
    QJsonObject obj;
    obj["cmd"] = "history";
    obj["name"] = name;
    obj["limit"] = "32768";
    obj["start"] = QString::number(QDateTime(start, QTime(0, 0, 0, 0)).toMSecsSinceEpoch());
    obj["end"] = QString::number(QDateTime(end, QTime(23, 59, 59, 999)).toMSecsSinceEpoch());
    writeJson(obj);
}

void Network::requestRequestFile(const QString &receiver, const QString &fileName, qint64 size) {
    QJsonObject obj;
    obj["cmd"] = "request_file";
    obj["receiver"] = receiver;
    obj["file_name"] = fileName;
    obj["size"] = QString::number(size);
    writeJson(obj);
}

void Network::requestAcceptFile(const QString &sender, quint16 port) {
    QJsonObject obj;
    obj["cmd"] = "response_file";
    obj["sender"] = sender;
    obj["port"] = QString::number(port);
    writeJson(obj);
}

void Network::requestRejectFile(const QString &sender) {
    QJsonObject obj;
    obj["cmd"] = "response_file";
    obj["sender"] = sender;
    obj["port"] = "-1";
    writeJson(obj);
}

void Network::requestChangeImg(const QString &imgName) {
    QJsonObject obj;
    obj["cmd"] = "change_img";
    obj["img"] = imgName;
    writeJson(obj);
}

void Network::requestGroupFileList(const QString &groupName) {
    QJsonObject obj;
    obj["cmd"] = "file_list";
    obj["group_name"] = groupName;
    writeJson(obj);
}

void Network::requestUploadFile(const QString &groupName, const QString &fileName, qint64 size, quint16 port) {
    QJsonObject obj;
    obj["cmd"] = "upload_file";
    obj["group_name"] = groupName;
    obj["file_name"] = fileName;
    obj["size"] = QString::number(size);
    obj["port"] = QString::number(port);
    writeJson(obj);
}

void Network::requestDownloadFile(const QString &groupName, const QString &fileName, quint16 port) {
    QJsonObject obj;
    obj["cmd"] = "download_file";
    obj["group_name"] = groupName;
    obj["file_name"] = fileName;
    obj["port"] = QString::number(port);
    writeJson(obj);
}

void Network::requestDeleteFile(const QString &groupName, const QString &fileName) {
    QJsonObject obj;
    obj["cmd"] = "delete_file";
    obj["group_name"] = groupName;
    obj["file_name"] = fileName;
    writeJson(obj);
}

void Network::requestImg(const QString &imgName, const QJsonObject &obj) {
    if (!imgJsonMap.contains(imgName)) {
        QJsonObject tmpObj;
        tmpObj["cmd"] = "request_img";
        tmpObj["img"] = imgName;
        writeJson(tmpObj);
    }
    imgJsonMap.insert(imgName, obj);
}

void Network::handleJson(const QJsonObject &obj) {
    QJsonDocument doc(obj);
    auto cmd = obj["cmd"].toString();
    if (cmd == "register") {
        auto status = obj["status"].toString();
        if (status == "success") {
            emit registerSuccessSignal();
        } else if (status == "fail") {
            emit registerFailSignal();
        }
    } else if (cmd == "login") {
        auto status = obj["status"].toString();
        if (status == "success") {
            auto imgName = obj["img"].toString();
            QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
            if (!file.exists()) {
                requestImg(imgName, obj);
            } else {
                auto list = obj["friends"].toArray();
                for (auto item : list) {
                    auto userObj = item.toObject();
                    auto imgName1 = userObj["img"].toString();
                    QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName1);
                    if (!file.exists()) {
                        requestImg(imgName1, obj);
                        return;
                    }
                }
                QList<User> userList;
                for (auto item : list) {
                    auto userObj = item.toObject();
                    userList.append({userObj["name"].toString(), userObj["img"].toString(), userObj["ip"].toString(), userObj["new_msg"].toBool()});
                }
                emit loginSuccessSignal(imgName, userList);
            }
        } else if (status == "unauthorized") {
            emit loginUnauthorizedSignal();
        } else if (status == "already") {
            emit loginAlreadySignal();
        }
    } else if (cmd == "group_member_list") {
        auto groupName = obj["group_name"].toString();
        auto members = obj["members"].toArray();
        QList<QString> list;
        for (auto item : members) {
            list.append(item.toString());
        }
        emit groupMemberListSignal(groupName, list);
    } else if (cmd == "create_group") {
        auto status = obj["status"].toString();
        if (status == "success") {
            emit createGroupSuccessSignal();
        } else if (status == "fail") {
            emit createGroupFailSignal();
        }
    } else if (cmd == "add") {
        auto status = obj["status"].toString();
        auto name = obj["name"].toString();
        if (status == "success") {
            auto imgName = obj["img"].toString();
            QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
            if (!file.exists()) {
                requestImg(imgName, obj);
            } else {
                auto ip = obj["ip"].toString();
                emit addFriendSuccessSignal(name, ip, imgName);
            }
        } else if (status == "fail") {
            emit addFriendFailSignal(name);
        }
    } else if (cmd == "be_added") {
        auto imgName = obj["img"].toString();
        QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
        if (!file.exists()) {
            requestImg(imgName, obj);
        } else {
            auto name = obj["name"].toString();
            auto ip = obj["ip"].toString();
            emit beAddedSignal(name, ip, imgName);
        }
    } else if (cmd == "be_deleted") {
        auto name = obj["name"].toString();
        emit beDeletedSignal(name);
    } else if (cmd == "online") {
        auto name = obj["name"].toString();
        auto ip = obj["ip"].toString();
        emit friendOnlineSignal(name, ip);
    } else if (cmd == "offline") {
        auto name = obj["name"].toString();
        emit friendOfflineSignal(name);
    } else if (cmd == "img_changed") {
        auto imgName = obj["img"].toString();
        QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
        if (!file.exists()) {
            requestImg(imgName, obj);
        } else {
            auto name = obj["name"].toString();
            emit friendImageChangedSignal(name, imgName);
        }
    } else if (cmd == "new_msg") {
        auto type = obj["type"].toString();
        auto msg = obj["msg"].toString();
        if (type == "img") {
            QFile file(QCoreApplication::applicationDirPath() + "/images/" + msg);
            if (!file.exists()) {
                requestImg(msg, obj);
                return;
            }
        }
        auto innerName = obj["inner_name"].toString();
        auto sender = obj["sender"].toString();
        emit newMsgSignal(innerName, sender, msg, type);
    } else if (cmd == "request_file") {
        auto sender = obj["sender"].toString();
        auto fileName = obj["file_name"].toString();
        auto size = obj["size"].toString().toLongLong();
        emit requestFileSignal(sender, fileName, size);
    } else if (cmd == "response_file") {
        auto receiver = obj["receiver"].toString();
        auto port = obj["port"].toString().toInt();
        if (port <= 0 || port > 65535) {
            emit rejectFileSignal(receiver);
        } else {
            emit acceptFileSignal(receiver, port);
        }
    } else if (cmd == "img") {
        auto imgName = obj["img"].toString();
        auto base64 = obj["base64"].toString();
        auto array = QByteArray::fromBase64(base64.toLatin1());
        QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
        file.open(QIODevice::WriteOnly);
        file.write(array);
        file.close();
        for (auto item : imgJsonMap.values(imgName)) {
            handleJson(item);
        }
        imgJsonMap.remove(imgName);
    } else if (cmd == "history") {
        auto records = obj["records"].toArray();
        for (auto item : records) {
            auto obj1 = item.toObject();
            auto type = obj1["type"].toString();
            auto msg = obj1["msg"].toString();
            if (type == "img") {
                QFile file(QCoreApplication::applicationDirPath() + "/images/" + msg);
                if (!file.exists()) {
                    requestImg(msg, obj);
                    return;
                }
            }
        }
        auto name = obj["name"].toString();
        QList<ChatRecord> list;
        for (auto item : records) {
            auto obj1 = item.toObject();
            list.append({obj1["sender"].toString(), obj1["msg"].toString(), obj1["type"].toString(), obj1["timestamp"].toString().toLongLong()});
        }
        emit historySignal(name, list);
    } else if (cmd == "file_list") {
        auto groupName = obj["group_name"].toString();
        auto files = obj["files"].toArray();
        QList<GroupFile> list;
        for (auto item : files) {
            auto obj1 = item.toObject();
            list.append({obj1["file_name"].toString(), obj1["uploader"].toString(), obj1["size"].toString().toLongLong(), obj1["timestamp"].toString().toLongLong()});
        }
        emit fileListSignal(groupName, list);
    } else if (cmd == "request_img") {
        auto imgName = obj["img"].toString();
        QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
        file.open(QIODevice::ReadOnly);
        auto array = file.readAll();
        file.close();
        QJsonObject obj1;
        obj1["cmd"] = "img";
        obj1["img"] = imgName;
        obj1["base64"] = QString(array.toBase64());
        writeJson(obj1);
    } else if (cmd == "send_msg") {
        auto receiver = obj["receiver"].toString();
        emit sendMsgSuccessSignal(receiver, msgCache[receiver].msg, msgCache[receiver].type);
        msgCache.remove(receiver);
    }
}

void Network::onReadyRead() {
    qint64 len;
    while ((len = socket.read(recvArr + recvLen, exceptLen - recvLen)) > 0) {
        recvLen += len;
        if (recvLen == exceptLen) {
            if (isReadyReadJson) {
                exceptLen = 4;
                recvArr[recvLen] = 0;
                // 开始处理
                QByteArray array;
                if (isCompressed) {
                    array = qUncompress(reinterpret_cast<unsigned char *>(recvArr), recvLen);
                } else {
                    array = QByteArray(recvArr, recvLen);
                }
                auto document = QJsonDocument::fromJson(array);
                handleJson(document.object());
            } else {
                exceptLen = *reinterpret_cast<int *>(recvArr);
                if (exceptLen < 0) {
                    isCompressed = true;
                    exceptLen = -exceptLen;
                } else {
                    isCompressed = false;
                }
                if (exceptLen > 4 * 1024 * 1024) {
                    return;
                }
            }
            isReadyReadJson = !isReadyReadJson;
            recvLen = 0;
        }
    }
}

void Network::onStateChanged() {
    if (socket.state() == QAbstractSocket::UnconnectedState) {
        timer.stop();
        emit disconnectedSignal();
    } else if (socket.state() == QAbstractSocket::ConnectedState) {
        timer.start();
    }
}
