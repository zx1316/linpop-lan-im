#ifndef GROUPFILEWINDOW_H
#define GROUPFILEWINDOW_H

#include <QWidget>
#include <QTimer>
#include "network.h"
#include "serverrecvthread.h"
#include "serversendthread.h"
#include "mihoyolauncher.h"

namespace Ui {
class GroupFileWindow;
}

class GroupFileWindow : public QWidget {
    Q_OBJECT

public:
    explicit GroupFileWindow(const QString &selfName, const QString &serverIp, MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~GroupFileWindow();
    void refreshFileList(const QList<GroupFile> &list);

private:
    Ui::GroupFileWindow *ui;
    QString selfName;
    QString serverIp;
    MiHoYoLauncher *launcher;
    ServerSendThread *sendThread = nullptr;
    ServerRecvThread *recvThread = nullptr;
    int selectedRow = -1;

private slots:
    void onRefreshClicked();
    void onUploadClicked();
    void onDownloadClicked();
    void onDeleteClicked();

    void onUploadFail();
    void onDownloadSuccess();
    void onDownloadFail();

signals:
    void groupFileQuerySignal();
    void groupFileDeleteSignal(QString fileName);
    void groupFileDownloadSignal(QString fileName, quint16 port);
    void groupFileUploadSignal(QString fileName, qint64 size, quint16 port);
    void windowClosed();
};

#endif // GROUPFILEWINDOW_H
