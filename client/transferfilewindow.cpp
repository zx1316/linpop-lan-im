#include "transferfilewindow.h"
#include "ui_transferfilewindow.h"
#include "QFileDialog"
#include "autostart.h"

TransferFileWindow::TransferFileWindow(QWidget *parent):
    QWidget(parent),
    ui(new Ui::TransferFileWindow)
{
    ui->setupUi(this);
    ui->transfer_file_button->setEnabled(false);
    ui->state_label->setText("请打开文件");
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(100);
    connect(ui->open_file_button,SIGNAL(clicked()),this,SLOT(onOpenFileButtonClicked()));
    connect(ui->transfer_file_button,SIGNAL(clicked()),this,SLOT(onTransferFileButtonClicked()));
    connect(ui->quit_button,SIGNAL(clicked()),this,SLOT(close()));
}

TransferFileWindow::~TransferFileWindow()
{
    delete ui;
}

void TransferFileWindow::closeEvent(QCloseEvent *event) {
    emit closeWindowSignal();
    if (thread != nullptr) {
        thread->interruptFlag = true;
        thread->quit();
        thread->wait();
        disconnect(thread);
        delete thread;
    }
}

void TransferFileWindow::onOpenFileButtonClicked() {
    gachaAutoStart(this);
    this->_url = QFileDialog::getOpenFileName(this, "打开文件", "../");
    if(!_url.isEmpty()){
        QString file_name = _url.right(_url.size()-_url.lastIndexOf('/')-1);
        ui->state_label->setText(file_name);
        ui->transfer_file_button->setEnabled(true);
    } else {
        ui->state_label->setText("请打开文件");
        ui->transfer_file_button->setEnabled(false);
    }
}

/**
 * 函数名称:onTransferFileButtonClicked
 * 功能描述:在点击发送文件时,向ChatWindow发送传输文件请求。（请求起点）
 */
void TransferFileWindow::onTransferFileButtonClicked() {
    gachaAutoStart(this);
    QString file_name = _url.right(_url.size()-_url.lastIndexOf('/')-1);
    qDebug("TransferFileWindow Transfer File Request");
    QFile file(_url);
    emit transferFileRequestSignal(file_name, file.size());
    ui->transfer_file_button->setEnabled(false);
    ui->open_file_button->setEnabled(false);
    ui->progressBar->setValue(0);
}
/**
 * 函数名称：transferFile()
 * 描述：新建子线程，在子线程中发送文件
 * 参数：void
 * 返回值:void
 * 做成时间：2023.8.26
 * 作者：刘文景
 */
void TransferFileWindow::onTransferFileFeedbackSignal(bool feedback,QString ip,int port){
    qDebug("TransferFileWindow Transfer File Feedback");
    if(!feedback) {
        QMessageBox::information(this,"Fail:","对方拒绝了你的传输申请");
        ui->state_label->setText("请打开文件");
        ui->open_file_button->setEnabled(true);
        ui->transfer_file_button->setEnabled(false);
    } else {
        ui->state_label->setText("正在传输文件");
        thread = new SendThread(ip, port, _url);
        ui->open_file_button->setEnabled(false);
        connect(thread, &SendThread::progress, this, &TransferFileWindow::sending_slot);
//        connect(thread, &SendThread::noServer, this, &TransferFileWindow::onNoServer);
        connect(thread, &SendThread::failed, this, &TransferFileWindow::onFailed);
        connect(thread, &SendThread::finish, this, &TransferFileWindow::onFinish);
        thread->start();
    }
}

void TransferFileWindow::sending_slot(const int current) {
    ui->progressBar->setValue(current);
}

void TransferFileWindow::onFinish() {
    thread->quit();
    thread->wait();
    disconnect(thread);
    delete thread;
    thread = nullptr;
    QMessageBox::information(this,"Success:","发送完毕");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
}
/*
void TransferFileWindow::onNoServer() {
    thread->quit();
    thread->wait();
    thread = nullptr;
    QMessageBox::information(this,"Failed:","发送端未能连接到接收端");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
}*/

void TransferFileWindow::onFailed() {
    thread->quit();
    thread->wait();
    disconnect(thread);
    delete thread;
    thread = nullptr;
    QMessageBox::information(this,"Fail:","发送失败");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
    ui->progressBar->setValue(0);
}
