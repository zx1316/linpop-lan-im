#include "receivefilewindow.h"
#include "ui_receivefilewindow.h"

ReceiveFileWindow::ReceiveFileWindow(QString path, QString name, QWidget *parent) : QWidget(parent), ui(new Ui::ReceiveFileWindow), sender(name) {
    ui->setupUi(this);
    //初始化进度条
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    this->setAttribute(Qt::WA_DeleteOnClose);
    thread = new RecvThread(path);
    connect(thread, &RecvThread::progress, this, &ReceiveFileWindow::updateProgress);
//    connect(thread, &RecvThread::noRequest, this, &ReceiveFileWindow::onNoRequest);
    connect(thread, &RecvThread::failed, this, &ReceiveFileWindow::onFailed);
    connect(thread, &RecvThread::availablePort, this, &ReceiveFileWindow::onGetPort);
    connect(thread, &RecvThread::finish, this, &ReceiveFileWindow::onFinish);
//    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

ReceiveFileWindow::~ReceiveFileWindow() {
    if (thread != nullptr) {
        thread->interruptFlag = true;
        thread->quit();
        thread->wait();
        disconnect(thread);
        delete thread;
    }
    delete ui;
}

void ReceiveFileWindow::updateProgress(const int current) {
    ui->progressBar->setValue(current);
}

void ReceiveFileWindow::onFinish() {
    QMessageBox::information(this,"Success:","接收完毕");
    thread->quit();
    thread->wait();
    disconnect(thread);
    delete thread;
    thread = nullptr;
    close();
}
/*
void ReceiveFileWindow::onNoRequest() {
    thread->quit();
    thread->wait();
    QMessageBox::information(this,"Failed:","未收到发送文件请求");
    close();
}*/

void ReceiveFileWindow::onFailed() {
    QMessageBox::information(this,"Failed:","接收失败");
    thread->quit();
    thread->wait();
    disconnect(thread);
    delete thread;
    thread = nullptr;
    close();
}

void ReceiveFileWindow::onGetPort(int port) {
    emit accept(sender, port);
}
