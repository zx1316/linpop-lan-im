#include "receivefilewindow.h"
#include "ui_receivefilewindow.h"
#include <QMessageBox>
#include <QTimer>

ReceiveFileWindow::ReceiveFileWindow(const QString& path, const QString& name, const QString& ip, QWidget *parent) : QWidget(parent), ui(new Ui::ReceiveFileWindow) {
    ui->setupUi(this);
    //初始化进度条
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    this->setAttribute(Qt::WA_DeleteOnClose);
    thread = new ServerRecvThread(path, ip);
    auto timer = new QTimer;
    timer->setInterval(30000);
    connect(thread, &ServerRecvThread::portAvailable, this, [=] (quint16 port) {
        emit accept(name, port);
    });
    connect(thread, &ServerRecvThread::progress, this, [=] (qint8 progress) {
        ui->progressBar->setValue(progress);
        timer->start();
    });
    connect(thread, &ServerRecvThread::success, this, &ReceiveFileWindow::onFinish);
    connect(thread, &ServerRecvThread::fail, this, &ReceiveFileWindow::onFail);
    connect(thread, &ServerRecvThread::finished, thread, &ServerRecvThread::deleteLater);
    connect(thread, &ServerRecvThread::finished, timer, &QTimer::deleteLater);
    connect(timer, &QTimer::timeout, this, &ReceiveFileWindow::onFail);
    timer->start();
    thread->start();
}

ReceiveFileWindow::~ReceiveFileWindow() {
    if (thread != nullptr) {
        thread->quit();
    }
    delete ui;
}

void ReceiveFileWindow::onFinish() {
    thread->quit();
    thread = nullptr;
    QMessageBox::information(this,"Success:","接收完毕");
    close();
}

void ReceiveFileWindow::onFail() {
    thread->quit();
    thread = nullptr;
    QMessageBox::critical(this,"Failed:","接收失败");
    close();
}

