/*
 * 文件名称：transferfilewindow.h
 * 类名称：TransferFileWindow（聊天记录窗口）
 * 描述：用于打开并传递文件
 * 成员变量：
 * 1.   ui:TransferFileWindow*   窗口界面
 * 2.   url:QString              保存文件路径
 * 初版完成时间：2023.08.23
 * 做成时间：----。--。--
 * 作者：林方裕
 */
#ifndef TRANSFERFILEWINDOW_H
#define TRANSFERFILEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QThread>
#include <QMessageBox>
#include "sendthread.h"

namespace Ui {
class TransferFileWindow;
}

class TransferFileWindow : public QWidget
{
    Q_OBJECT
    QThread _thread;

public:
    explicit TransferFileWindow(QWidget *parent = nullptr);
    ~TransferFileWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::TransferFileWindow *ui;
    QString _url;
    SendThread *thread = nullptr;
    QString _receiver_ip;
    int _port;

public slots:
    void onOpenFileButtonClicked();
    void onTransferFileButtonClicked();
    void onTransferFileFeedbackSignal(bool, QString,int port);
    void sending_slot(const int);
    void onFailed();
//    void onNoServer();
    void onFinish();

signals:
    void closeWindowSignal();
    void operate();
    void transferFileRequestSignal(QString, qint64);
};

#endif // TRANSFERFILEWINDOW_H
