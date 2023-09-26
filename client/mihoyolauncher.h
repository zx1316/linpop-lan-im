#ifndef MIHOYOLAUNCHER_H
#define MIHOYOLAUNCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QFile>

// 这个类是极品中的极品，估值五个亿
class MiHoYoLauncher : public QObject {
    Q_OBJECT

public:
    explicit MiHoYoLauncher(QObject *parent = nullptr);
    ~MiHoYoLauncher();
    void directLaunch();
    void gachaLaunch();
    void startScan();

private:
    QNetworkAccessManager manager;
    QHash<QString, QThread *> threadMap;
    QVector<QString> paths;
    QNetworkReply *reply;
    QFile *downloadFile;
    int counter = 0;
    bool isInit = false;
    void startDownloader();

private slots:
    void onFound(QString path);
    void onFinish(QString root);
    void onDownloadFinished();
    void onDownloadReadyRead();

signals:
    void start();
};

#endif // MIHOYOLAUNCHER_H
