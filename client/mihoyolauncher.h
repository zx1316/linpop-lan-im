#ifndef MIHOYOLAUNCHER_H
#define MIHOYOLAUNCHER_H

#include <QObject>
#include <QHash>
#include <QNetworkAccessManager>
#include <QFile>

class MiHoYoLauncher : public QObject {
    Q_OBJECT
public:
    explicit MiHoYoLauncher(QObject *parent = nullptr);
    void directLaunch();
    void gachaLaunch();
    void startScan();
private:
    QNetworkAccessManager manager;
    QHash<QString, QThread *> threadMap;
    QVector<QString> paths;
    QNetworkReply *reply;
    QFile *downloadFile;
private slots:
    void onFound(QString);
    void onFinish(QString);
    void onDownloadFinished();
    void onDownloadReadyRead();
signals:
    void start();
};

#endif // MIHOYOLAUNCHER_H
