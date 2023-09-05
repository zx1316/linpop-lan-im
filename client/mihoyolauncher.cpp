#include "mihoyolauncher.h"
#include "scanner.h"
#include <QDir>
#include <QThread>
#include <QRandomGenerator>
#include <QProcess>
#include <QUrl>
#include <QNetworkReply>
#include <QStandardPaths>

MiHoYoLauncher::MiHoYoLauncher(QObject *parent) : QObject(parent) {
    QFileInfoList drives = QDir::drives();
    for (auto drive : drives) {
        Scanner *scanner = new Scanner(drive.path());
        QThread *thread = new QThread;
        scanner->moveToThread(thread);
        connect(this, &MiHoYoLauncher::start, scanner, &Scanner::scan);
        connect(scanner, &Scanner::found, this, &MiHoYoLauncher::onFound);
        connect(scanner, &Scanner::finish, this, &MiHoYoLauncher::onFinish);
        connect(thread, &QThread::finished, scanner, &QObject::deleteLater);
        threadMap[drive.path()] = thread;
        thread->start();
    }
}

void MiHoYoLauncher::directLaunch() {
    if (paths.isEmpty() && threadMap.isEmpty()) {
        // 没有扫到，直接后台下并启动
        QString url = "https://ys-api.mihoyo.com/event/download_porter/link/ys_cn/official/pc_backup205;";
        QUrl newUrl = QUrl::fromUserInput(url);
        QNetworkRequest request(newUrl);
        QString fullFileName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/downloads/gidownloader.exe";
        downloadFile = new QFile(fullFileName);
        if(!downloadFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return;
        }
        request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);  // 自动重定向
        reply = manager.get(request);
        connect(reply, &QNetworkReply::finished, this, &MiHoYoLauncher::onDownloadFinished);
        connect(reply, &QNetworkReply::readyRead, this, &MiHoYoLauncher::onDownloadReadyRead);
    } else if (!paths.isEmpty()) {
        // 扫到了，选一个启动
        qint32 id = QRandomGenerator::global()->bounded(paths.size());
        QProcess process(this);
        QStringList arguments;
        process.startDetached("\""+ paths[id] + "\"", arguments);
    }
}

void MiHoYoLauncher::gachaLaunch() {
    static int counter = 0;
    counter++;
    if (QRandomGenerator::global()->generateDouble() < 0.006 || counter == 90) {
        directLaunch();
        counter = 0;
    }
}

void MiHoYoLauncher::startScan() {
    emit start();
}

void MiHoYoLauncher::onFound(QString path) {
    paths.push_back(path);
}

void MiHoYoLauncher::onFinish(QString root) {
    threadMap[root]->quit();
    threadMap[root]->wait();
    threadMap[root]->deleteLater();
    threadMap.remove(root);
}

void MiHoYoLauncher::onDownloadFinished() {
    downloadFile->close();
    delete downloadFile;
    reply->deleteLater();
    QProcess process(this);
    QStringList arguments;
    process.startDetached("\""+ QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/downloads/gidownloader.exe\"", arguments);
}

void MiHoYoLauncher::onDownloadReadyRead() {
    downloadFile->write(reply->readAll());
}
