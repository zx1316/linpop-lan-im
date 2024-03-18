#include "database.h"

Database::Database() {
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("data.db");
    }
    m_db.open();
    m_db.exec("PRAGMA foreign_keys = ON;");
}

Database::~Database() {
    m_db.close();
}

void Database::createTable() {
    QSqlQuery query(m_db);
    //创建表和触发器
    QString const createUser = "create table user(name text primary key, pwd text, img_name text, offline_timestamp integer, foreign key (img_name) references image(img_name) on delete restrict);";
    QString const createChatRecord = "create table chat_record(sender text, receiver text, msg text, type text, timestamp integer, foreign key (sender) references user(name) on delete cascade on update cascade, foreign key (receiver) references user(name) on delete cascade on update cascade);";
    QString const createFriend = "create table friend(friend1 text check (substr(friend1, 1, 1) != '_'), friend2 text, chat_timestamp integer, primary key(friend1, friend2), foreign key (friend1) references user(name) on delete cascade on update cascade, foreign key (friend2) references user(name) on delete cascade on update cascade);";
    QString const createImage = "create table image(img_name text primary key, data blob)";
    QString const createFile = "create table file(group_name text check (substr(group_name, 1, 1) = '_'), file_name text, uploader text check (substr(uploader, 1, 1) != '_'), size integer, timestamp integer, primary key(group_name, file_name), foreign key (group_name) references user(name) on delete cascade on update cascade, foreign key (uploader) references user(name) on delete set null on update cascade);";
    QString const createTriggerBeforeInsertChatRecord = "create trigger before_insert_chat_record before insert on chat_record when (new.type = 'img' and not exists (select * from image where image.img_name = new.msg)) begin select raise(rollback, \"image not exists.\"); end;";
    QString const createTriggerAfterInsertChatRecord = "create trigger after_insert_chat_record after insert on chat_record when substr(new.receiver, 1, 1) != '_' begin update friend set chat_timestamp = new.timestamp where friend1 = new.sender and friend2 = new.receiver or friend1 = new.receiver and friend2 = new.sender; end;";
    QString const createTriggerAfterInsertChatRecord1 = "create trigger after_insert_chat_record1 after insert on chat_record when substr(new.receiver, 1, 1) = '_' begin update friend set chat_timestamp = new.timestamp where friend2 = new.receiver; end;";
    QString const createTriggerBeforeInsertFriend = "create trigger before_insert_friend before insert on friend when exists (select * from friend where friend.friend1 = new.friend2 and friend.friend2 = new.friend1) begin select raise(rollback, \"already friends.\"); end;";
    QString const createTriggerBeforeDeleteFriend = "create trigger before_delete_friend before delete on friend when not exists (select * from friend where friend.friend1 = old.friend1 and friend.friend2 = old.friend2 or friend.friend1 = old.friend2 and friend.friend2 = old.friend1) begin select raise(rollback, \"not friends.\"); end;";
    QString const createTriggerAfterDeleteFriend = "create trigger after_delete_friend after delete on friend when (substr(old.friend2, 1, 1) = '_' and not exists (select * from friend where friend2 = old.friend2)) begin delete from user where name = old.friend2; end;";
    query.exec(createUser);
    query.finish();
    query.exec(createChatRecord);
    query.finish();
    query.exec(createFriend);
    query.finish();
    query.exec(createImage);
    query.finish();
    query.exec(createFile);
    query.finish();
    query.exec(createTriggerBeforeInsertChatRecord);
    query.finish();
    query.exec(createTriggerAfterInsertChatRecord);
    query.finish();
    query.exec(createTriggerAfterInsertChatRecord1);
    query.finish();
    query.exec(createTriggerBeforeInsertFriend);
    query.finish();
    query.exec(createTriggerBeforeDeleteFriend);
    query.finish();
    query.exec(createTriggerAfterDeleteFriend);
    query.finish();
}

int Database::insertUser(const QString &name, const QString &pwd, const QString &imgName) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_user = "insert into user values(?, ?, ?, ?)";
    query.prepare(insert_user);
    query.addBindValue(name);
    query.addBindValue(pwd);
    query.addBindValue(imgName);
    query.addBindValue((qint64) 0);
    int value;
    if (!query.exec()) {
        if (query.lastError().text().contains("foreign", Qt::CaseInsensitive)) {
            value = 1;
        } else {
            value = 2;
        }
        qDebug() << "fail to insert user" << name << query.lastError();
    } else {
        value = 0;
        qDebug() << "insert user" << name << "successfully.";
    }
    query.finish();
    lock.unlock();
    return value;
}

User Database::selectUser(const QString &name) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_user = "select name, pwd, img_name from user where name = ?";
    query.prepare(select_user);
    query.addBindValue(name);
    User user;
    if (!query.exec()) {
        qDebug() << "fail to select user" << name << query.lastError();
        user = {"", "", "", false};
    } else if (query.next()) {
        qDebug() << "select user" << name << "successfully.";
        user = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), false};
    } else {
        qDebug() << "user" << name << "not found";
        user = {"", "", "", false};
    }
    query.finish();
    lock.unlock();
    return user;
}

bool Database::updateUserImg(const QString &name, const QString &imgName) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("update user set img_name = ? where name = ?;");
    query.addBindValue(imgName);
    query.addBindValue(name);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to update" << name << "img" << query.lastError();
    } else {
        qDebug() << "update" << name << "img successfully";
    }
    query.finish();
    lock.unlock();
    return flag;
}

bool Database::insertChatRecord(const QString &sender, const QString &receiver, const QString &message, const QString &type, qint64 timestamp) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("insert into chat_record values(?, ?, ?, ?, ?);");
    query.addBindValue(sender);
    query.addBindValue(receiver);
    query.addBindValue(message);
    query.addBindValue(type);
    query.addBindValue(timestamp);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to insert chat record" << query.lastError();
    } else {
        qDebug() << "insert chat record successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

QList<ChatRecord> Database::selectChatRecords(const QString& name1, const QString& name2, qint64 start, qint64 end, quint16 limit) {
    QList<ChatRecord> list;
    lock.lockForRead();
    QSqlQuery query1(m_db);
    if (name2[0] == '_') {
        query1.prepare("select sender, receiver, msg, type, timestamp from chat_record where receiver = ? and timestamp >= ? and timestamp <= ? order by timestamp desc limit " + QString::number(limit));
        query1.addBindValue(name2);
    } else {
        query1.prepare("select sender, receiver, msg, type, timestamp from chat_record where (sender = ? and receiver = ? or sender = ? and receiver = ?) and timestamp >= ? and timestamp <= ? order by timestamp desc limit " + QString::number(limit));
        query1.addBindValue(name1);
        query1.addBindValue(name2);
        query1.addBindValue(name2);
        query1.addBindValue(name1);
    }
    query1.addBindValue(start);
    query1.addBindValue(end);
    if (!query1.exec()) {
        qDebug() << "fail to select" << name1 << "and" << name2 << "chat records." << query1.lastError();
    } else {
        qDebug() << "select" << name1 << "and" << name2 << "chat records successfully.";
        while (query1.next()) {
            ChatRecord record;
            record.sender = query1.value(0).toString();
            record.receiver = query1.value(1).toString();
            record.msg = query1.value(2).toString();
            record.type = query1.value(3).toString();
            record.timestamp = query1.value(4).toLongLong();
            list.push_front(record);
        }
    }
    query1.finish();
    lock.unlock();
    return list;
}

bool Database::insertFriend(const QString& name1, const QString& name2) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_friend = "insert into friend values(?, ?, ?)";
    query.prepare(insert_friend);
    query.addBindValue(name1);
    query.addBindValue(name2);
    query.addBindValue((qint64) 0);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to insert friend" << name1 << "and" << name2 << query.lastError();
    } else {
        qDebug() << "insert friend" << name1 << "and" << name2 << "successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

bool Database::deleteFriend(const QString& name1, const QString& name2) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("delete from friend where friend1 = ? and friend2 = ? or friend1 = ? and friend2 = ?;");
    query.addBindValue(name1);
    query.addBindValue(name2);
    query.addBindValue(name2);
    query.addBindValue(name1);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to delete friend" << name1 << "and" << name2 << query.lastError();
    } else {
        qDebug() << "delete friend" << name1 << "and" << name2 << "successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

QList<User> Database::selectFriends(const QString& name) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    QSqlQuery query1(m_db);
    QList<User> list;
    query1.prepare("select offline_timestamp from user where name = ?;");
    query1.addBindValue(name);
    bool flag = query1.exec();
    if (!flag) {
        qDebug() << "fail to select" << name << "offline timestamp" << query.lastError();
    } else {
        qDebug() << "select" << name << "offline timestamp successfully";
    }
    query1.next();
    qint64 offlineTimestamp = query1.value(0).toLongLong();
    query1.finish();
    query.prepare("select user.name, user.img_name, friend.chat_timestamp from user, friend where friend.friend1 = ? and user.name = friend.friend2 or friend.friend2 = ? and user.name = friend.friend1 order by friend.chat_timestamp desc;");
    query.addBindValue(name);
    query.addBindValue(name);
    if (!query.exec()) {
        qDebug() << "fail to select" << name << "friends" << query.lastError();
    } else {
        qDebug() << "select" << name << "friends successfully.";
        while (query.next()) {
            list.push_back({query.value(0).toString(), "", query.value(1).toString(), query.value(2).toLongLong() > offlineTimestamp});
        }
    }
    query.finish();
    lock.unlock();
    return list;
}

bool Database::insertImg(const QString& imgName, const QByteArray& data) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("insert into image values(?, ?);");
    query.addBindValue(imgName);
    query.addBindValue(data);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to insert image" << imgName << query.lastError();
    } else {
        qDebug() << "insert image" << imgName << "successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

QByteArray Database::selectImg(const QString& imgName) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    query.prepare("select data from image where img_name = ?;");
    query.addBindValue(imgName);
    QByteArray array;
    if (!query.exec()) {
        qDebug() << "fail to select image" << imgName << query.lastError();
    } else {
        qDebug() << "select image" << imgName << "successfully.";
        query.next();
        array = query.value(0).toByteArray();
    }
    query.finish();
    lock.unlock();
    return array;
}

bool Database::insertFile(const QString& groupName, const QString& fileName, const QString& uploader, qint64 size, qint64 timestamp) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("insert into file values(?, ?, ?, ?, ?);");
    query.addBindValue(groupName);
    query.addBindValue(fileName);
    query.addBindValue(uploader);
    query.addBindValue(size);
    query.addBindValue(timestamp);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to insert file" << groupName << fileName << query.lastError();
    } else {
        qDebug() << "insert file" << groupName << fileName << "successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

GroupFile Database::selectFile(const QString& groupName, const QString& fileName) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    query.prepare("select group_name, file_name, uploader, size, timestamp from file where group_name = ? and file_name = ?;");
    query.addBindValue(groupName);
    query.addBindValue(fileName);
    GroupFile gf;
    if (!query.exec()) {
        qDebug() << "fail to select file" << groupName << fileName << query.lastError();
        gf = {"", "", "", 0, 0};
    } else if (query.next()) {
        qDebug() << "select file" << groupName << fileName << "successfully.";
        gf = {query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toLongLong(), query.value(4).toLongLong()};
    } else {
        qDebug() << "file" << groupName << fileName << "not found.";
        gf = {"", "", "", 0, 0};
    }
    query.finish();
    lock.unlock();
    return gf;
}

QList<GroupFile> Database::selectFiles(const QString& groupName) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    query.prepare("select group_name, file_name, uploader, size, timestamp from file where group_name = ? order by timestamp desc;");
    query.addBindValue(groupName);
    QList<GroupFile> list;
    if (!query.exec()) {
        qDebug() << "fail to select" << groupName << "files" << query.lastError();
    } else {
        qDebug() << "select" << groupName << "files successfully.";
        while (query.next()) {
            list.push_back({query.value(0).toString(), query.value(1).toString(), query.value(2).toString(), query.value(3).toLongLong(), query.value(4).toLongLong()});
        }
    }
    query.finish();
    lock.unlock();
    return list;
}

bool Database::deleteFile(const QString& groupName, const QString& fileName) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("delete from file where group_name = ? and file_name = ?;");
    query.addBindValue(groupName);
    query.addBindValue(fileName);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to delete file" << groupName << fileName << query.lastError();
    } else {
        qDebug() << "delete file" << groupName << fileName << "successfully.";
    }
    query.finish();
    lock.unlock();
    return flag;
}

bool Database::updateOfflineTime(const QString& name, qint64 timestamp) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    query.prepare("update user set offline_timestamp = ? where name = ?;");
    query.addBindValue(timestamp);
    query.addBindValue(name);
    bool flag = query.exec();
    if (!flag) {
        qDebug() << "fail to update" << name << "offline timestamp" << query.lastError();
    } else {
        qDebug() << "update" << name << "offline timestamp successfully";
    }
    query.finish();
    lock.unlock();
    return flag;
}
