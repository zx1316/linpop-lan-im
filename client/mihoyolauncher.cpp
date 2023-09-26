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
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        threadMap[drive.path()] = thread;
        thread->start();
    }
}

MiHoYoLauncher::~MiHoYoLauncher() {
    for (auto item : threadMap) {
        item->quit();
        item->wait();
        item->deleteLater();
    }
}

void MiHoYoLauncher::directLaunch() {
    if (paths.isEmpty() && threadMap.isEmpty()) {
        // 没有扫到
        QString fullFileName = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/downloads/gidownloader.exe";
        if (QFile::exists(fullFileName)) {
            startDownloader();
        } else {
            // 这个网址自己搞以下好吧，谁知道几个月后下载地址有没有变
            QString url = "https://ys-api.mihoyo.com/event/download_porter/link/ys_cn/official/pc_backup205;";
            QUrl newUrl = QUrl::fromUserInput(url);
            QNetworkRequest request(newUrl);
            downloadFile = new QFile(fullFileName);
            if(!downloadFile->open(QIODevice::WriteOnly)) {
                return;
            }
            request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);  // 自动重定向
            reply = manager.get(request);
            connect(reply, &QNetworkReply::finished, this, &MiHoYoLauncher::onDownloadFinished);
            connect(reply, &QNetworkReply::readyRead, this, &MiHoYoLauncher::onDownloadReadyRead);
        }
    } else if (!paths.isEmpty()) {
        // 扫到了
        qint32 id = QRandomGenerator::global()->bounded(paths.size());
        QProcess process(this);
        QStringList arguments;
        process.startDetached("\""+ paths[id] + "\"", arguments);
    }
}

void MiHoYoLauncher::gachaLaunch() {
    counter++;
    if (QRandomGenerator::global()->generateDouble() < 0.006 || counter == 90) {
        directLaunch();
        counter = 0;
    }
}

// 检测"AppData/LocalLow/miHoYo/原神"文件夹是否存在可以快速检测是否安装，他家的其他游戏也是一样的，这里没有预检测，感兴趣可以加上
void MiHoYoLauncher::startScan() {
    if (!isInit) {
        isInit = true;
        emit start();
    }
}

void MiHoYoLauncher::onFound(QString path) {
    paths.push_back(path);
}

void MiHoYoLauncher::onFinish(QString root) {
    threadMap[root]->quit();
    threadMap[root]->wait();
    threadMap.remove(root);
}

void MiHoYoLauncher::onDownloadFinished() {
    downloadFile->close();
    delete downloadFile;
    reply->deleteLater();
    startDownloader();
}

void MiHoYoLauncher::onDownloadReadyRead() {
    downloadFile->write(reply->readAll());
}

void MiHoYoLauncher::startDownloader() {
    QProcess process(this);
    QStringList arguments;
    process.startDetached("\""+ QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/downloads/gidownloader.exe\"", arguments);
}
