#include "database.h"

Database::Database() {
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        m_db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName("BitChat.db");
    }
}

Database::~Database() {
    closeDatabase();
}

void Database::openDatabase() {
    if(!m_db.isOpen()) {
        m_db.open();
    }
}

void Database::closeDatabase() {
    if(m_db.isOpen()) {
        m_db.close();
    }
}

bool Database::createTable() {
    QSqlQuery query(m_db);
    //创建表 其中具体类型有待修改
    QString create_user = "create table user(username text primary key, passwordhash text)";
    QString create_history = "create table history(sender text, receiver text, timestamp integer, message text, fontsize integer, color integer, style integer)";
    QString create_friend = "create table friend(friendA text , friendB text, primary key(friendA, friendB))";
    if(query.exec(create_user)) {
        qDebug() << "table user created successfully";
        query.finish();
    } else {
        qDebug() << "failed to create table user" << query.lastError();
        query.finish();
    }
    if(query.exec(create_history)) {
        qDebug() << "table history created successfully";
        query.finish();
    } else {
        qDebug() << "failed to create table history" << query.lastError();
        query.finish();
    }
    if(query.exec(create_friend)) {
        qDebug() << "table friend created successfully";
        query.finish();
    } else {
        qDebug() << "failed to create table friend" << query.lastError();
        query.finish();
    }
    return true;
}

bool Database::insertUser(QString username, QString passwordhash, QReadWriteLock &lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_user = "insert into user values(?, ?)";
    query.prepare(insert_user);
    query.addBindValue(username);
    query.addBindValue(passwordhash);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "insert user" << username << "successfully. current access db thread:" << QThread::currentThread();
    }
    query.finish();
    lock.unlock();
    return flag;
}

User Database::selectUser(QString name, QReadWriteLock &lock) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_user = "select * from user where username = ?";
    query.prepare(select_user);
    query.addBindValue(name);
    User user;
    if(!query.exec()) {
        qDebug() << query.lastError();
        user = {"", ""};
    } else if (query.next()) {
        QString name = query.value(0).toString();
        QString pwd = query.value(1).toString();
        qDebug() << "select user" << name << "successfully. current access db thread:" << QThread::currentThread();
        user = {name, pwd};
    } else {
        qDebug() << "user not found";
        user = {"", ""};
    }
    query.finish();
    lock.unlock();
    return user;
}

bool Database::deleteUser(QString name, QReadWriteLock &lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_history = "DELETE FROM user WHERE username = ?;";
    query.prepare(insert_history);
    query.addBindValue(name);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "insert history successfully. current access db thread:" << QThread::currentThread();
    }
    query.finish();
    lock.unlock();
    return flag;
}

bool Database::insertHistory(QString se, QString re, QString mes, int font, int style, int color, qint64 dt, QReadWriteLock &lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_history = "insert into history values(?, ?, ?, ?, ?, ?, ?)";
    query.prepare(insert_history);
    query.addBindValue(se);
    query.addBindValue(re);
    query.addBindValue(dt);
    query.addBindValue(mes);
    query.addBindValue(font);
    query.addBindValue(color);
    query.addBindValue(style);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "insert history successfully. current access db thread:" << QThread::currentThread();
    }
    query.finish();
    lock.unlock();
    return flag;
}

QVector<History> Database::selectLatestHistory(QString userA, QString userB, QReadWriteLock &lock) {
    QVector<History> vec;
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_history = "SELECT * FROM history WHERE (sender = ? AND receiver = ? OR sender = ? AND receiver = ?) ORDER BY timestamp DESC LIMIT 50;";
    query.prepare(select_history);
    query.addBindValue(userA);
    query.addBindValue(userB);
    query.addBindValue(userB);
    query.addBindValue(userA);
    if(!query.exec()) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "select" << userA << "and" << userB << "latetst history successfully. current access db thread:" << QThread::currentThread();
        while(query.next()) {
            QString sender = query.value(0).toString();
            QString receiver = query.value(1).toString();
            qint64 datetime = query.value(2).toLongLong();
            QString message = query.value(3).toString();
            quint8 fontsize = query.value(4).toInt();
            int color = query.value(5).toInt();
            quint8 style = query.value(6).toInt();
            vec.push_back(History{sender, receiver, message, fontsize, style, color, datetime});
        }
    }
    query.finish();
    lock.unlock();
    return vec;
}

QVector<History> Database::selectLatestGroupHistory(QString userA, QReadWriteLock &lock) {
    QVector<History> vec;
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_history = "SELECT * FROM history WHERE receiver = ? ORDER BY timestamp DESC LIMIT 50;";

    query.prepare(select_history);
    query.addBindValue(userA);
    if(!query.exec()) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "select" << userA << "latest group history successfully. current access db thread:" << QThread::currentThread();
        while(query.next()) {
            QString sender = query.value(0).toString();
            QString receiver = query.value(1).toString();
            qint64 datetime = query.value(2).toLongLong();
            QString message = query.value(3).toString();
            quint8 fontsize = query.value(4).toInt();
            int color = query.value(5).toInt();
            quint8 style = query.value(6).toInt();
            vec.push_back(History{sender, receiver, message, fontsize, style, color, datetime});
        }
    }
    query.finish();
    lock.unlock();
    return vec;
}

QVector<History> Database::selectHistory(QString userA, QString userB, qint64 start, qint64 end, QReadWriteLock &lock) {
    QVector<History> vec;
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_history = "SELECT * FROM history WHERE ((sender = :userA AND receiver = :userB OR sender = :userB AND receiver = :userA) AND timestamp >= :start AND timestamp <= :end) ORDER BY timestamp DESC LIMIT 32767;";

    query.prepare(select_history);
    query.bindValue(":userA", userA);
    query.bindValue(":userB", userB);
    query.bindValue(":start", start);
    query.bindValue(":end", end);
    if(!query.exec()) {
        qDebug() << "select history failed" << query.lastError();
    } else {
        qDebug() << "select" << userA << "and" << userB << "history successfully. current access db thread:" << QThread::currentThread();
        while(query.next()) {
            QString sender = query.value(0).toString();
            QString receiver = query.value(1).toString();
            qint64 datetime = query.value(2).toLongLong();
            QString message = query.value(3).toString();
            quint8 fontsize = query.value(4).toInt();
            int color = query.value(5).toInt();
            quint8 style = query.value(6).toInt();
            vec.push_back(History{sender, receiver, message, fontsize, style, color, datetime});
        }
    }
    query.finish();
    lock.unlock();
    return vec;
}

QVector<History> Database::selectGroupHistory(QString userA, qint64 start, qint64 end, QReadWriteLock &lock) {
    qDebug() << "current access db thread:" << QThread::currentThread();

    QVector<History> vec;
    lock.lockForRead();
    QSqlQuery query(m_db);
    QString select_history = "SELECT * FROM history WHERE (receiver = :userA and timestamp >= :start and timestamp <= :end) ORDER BY timestamp DESC LIMIT 32767;";
    query.prepare(select_history);
    query.bindValue(":userA", userA);
    query.bindValue(":start", start);
    query.bindValue(":end", end);

    if(!query.exec()) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "select group history successfully";
        qDebug() << "select" << userA << "group history successfully. current access db thread:" << QThread::currentThread();
        while(query.next()) {
            QString sender = query.value(0).toString();
            QString receiver = query.value(1).toString();
            qint64 datetime = query.value(2).toLongLong();
            QString message = query.value(3).toString();
            quint8 fontsize = query.value(4).toInt();
            int color = query.value(5).toInt();
            quint8 style = query.value(6).toInt();
            vec.push_back(History{sender, receiver, message, fontsize, style, color, datetime});
        }
    }
    query.finish();
    lock.unlock();
    return vec;
}

bool Database::insertFriend(QString friendA, QString friendB, QReadWriteLock &lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString insert_friend = "insert into friend values(?, ?)";
    query.prepare(insert_friend);
    query.addBindValue(friendA);
    query.addBindValue(friendB);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "insert friend" << friendA << "and" << friendB << "successfully. current access db thread:" << QThread::currentThread();
    }
    query.finish();
    lock.unlock();
    return flag;
}

bool Database::deleteFriend(QString friendA, QString friendB, QReadWriteLock &lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString delete_friend = "DELETE FROM friend WHERE "
                           "(friendA = :friendA AND friendB = :friendB) OR "
                           "(friendA = :friendB AND friendB = :friendA)";
    query.prepare(delete_friend);
    query.bindValue(":friendA", friendA);
    query.bindValue(":friendB", friendB);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "delete friend" << friendA << "and" << friendB << "successfully. current access db thread:" << QThread::currentThread();
    }
    query.finish();
    lock.unlock();
    return flag;
}

QVector<QString> Database::selectFriend(QString userA, QReadWriteLock &lock) {
    lock.lockForRead();
    QSqlQuery query(m_db);
    QVector<QString> vec;
    QString select_friend = "SELECT username FROM user, friend WHERE "
                           "(friendA = :userA AND username = friendB) OR "
                           "(friendB = :userA AND username = friendA)";
    query.prepare(select_friend);
    query.bindValue(":userA",userA);
    if(!query.exec()) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "select" << userA << "friend successfully. current access db thread:" << QThread::currentThread();
        while(query.next()) {
            QString name = query.value(0).toString();
            vec.push_back(name);
        }
    }
    query.finish();
    lock.unlock();
    return vec;
}

bool Database::deleteGroupHistory(QString name, QReadWriteLock & lock) {
    lock.lockForWrite();
    QSqlQuery query(m_db);
    QString delete_friend = "DELETE FROM history WHERE receiver = ?;";
    query.prepare(delete_friend);
    query.addBindValue(name);
    bool flag = query.exec();
    if(!flag) {
        qDebug() << query.lastError();
    } else {
        qDebug() << "delete the history of group" << name << "successfully";
    }
    query.finish();
    lock.unlock();
    return flag;
}
