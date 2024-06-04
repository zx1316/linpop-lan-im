#ifndef GROUPFILEWINDOW_H
#define GROUPFILEWINDOW_H

#include <QWidget>
#include <QTimer>
#include "network.h"
#include "serverrecvthread.h"
#include "serversendthread.h"

namespace Ui {
class GroupFileWindow;
}

class GroupFileWindow : public QWidget {
    Q_OBJECT

public:
    GroupFileWindow(const QString& selfName, const QString& serverIp, QWidget *parent = nullptr);
    ~GroupFileWindow();
    void refreshFileList(const QList<GroupFile>& list);

private:
    Ui::GroupFileWindow *ui;
    QString selfName;
    QString serverIp;
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
    void groupFileDeleteSignal(const QString& fileName);
    void groupFileDownloadSignal(const QString& fileName, quint16 port);
    void groupFileUploadSignal(const QString& fileName, qint64 size, quint16 port);
    void windowClosed();
};

#endif // GROUPFILEWINDOW_H
