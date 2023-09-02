#ifndef RECEIVEFILEWINDOW_H
#define RECEIVEFILEWINDOW_H

#include <QWidget>
#include <QThread>
#include <QMessageBox>
#include "recvthread.h"

namespace Ui {
class ReceiveFileWindow;
}

class ReceiveFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiveFileWindow(QString path, QString sender, QWidget *parent = nullptr);
    ~ReceiveFileWindow();

public slots:
    void updateProgress(int);
    void onFailed();
//    void onNoRequest();
    void onGetPort(int);
    void onFinish();

private:
    Ui::ReceiveFileWindow *ui;
    RecvThread *thread = nullptr;
    QString sender;

signals:
    void accept(QString, int);
};

#endif // RECEIVEFILEWINDOW_H
