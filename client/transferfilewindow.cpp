#include "transferfilewindow.h"
#include "ui_transferfilewindow.h"
#include "QFileDialog"
#include <QTimer>

TransferFileWindow::TransferFileWindow(MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::TransferFileWindow), launcher(launcher) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->transfer_file_button->setEnabled(false);
    ui->state_label->setText("请打开文件");
    ui->progressBar->setValue(0);
    connect(ui->open_file_button,SIGNAL(clicked()),this,SLOT(onOpenFileButtonClicked()));
    connect(ui->transfer_file_button,SIGNAL(clicked()),this,SLOT(onTransferFileButtonClicked()));
    connect(ui->quit_button,SIGNAL(clicked()),this,SLOT(close()));
}

TransferFileWindow::~TransferFileWindow() {
    // 清理线程
    if (thread != nullptr) {
        thread->quit();
    }
    emit windowClosed();
    delete ui;
}

void TransferFileWindow::onOpenFileButtonClicked() {
    launcher->gachaLaunch();
    path = QFileDialog::getOpenFileName(this, "打开文件", "../");
    if(!path.isEmpty()) {
        ui->state_label->setText(QFileInfo(path).fileName());
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
    launcher->gachaLaunch();
    QFileInfo info(path);
    emit transferFileRequestSignal(info.fileName(), info.size());
    ui->transfer_file_button->setEnabled(false);
    ui->open_file_button->setEnabled(false);
}

void TransferFileWindow::onAcceptTransferFileSignal(const QString &ip, int port) {
    ui->state_label->setText("正在传输文件");
    thread = new ClientSendThread(path, ip, port);
    auto timer = new QTimer;
    timer->setInterval(30000);
    connect(thread, &ClientSendThread::progress, this, [=] (qint8 progress) {
        ui->progressBar->setValue(progress);
        timer->start();
    });
    connect(thread, &ClientSendThread::success, this, &TransferFileWindow::onFinish);
    connect(thread, &ClientSendThread::fail, this, &TransferFileWindow::onFail);
    connect(thread, &ClientSendThread::finished, thread, &ClientSendThread::deleteLater);
    connect(thread, &ClientSendThread::finished, timer, &QTimer::deleteLater);
    connect(timer, &QTimer::timeout, this, &TransferFileWindow::onFail);
    timer->start();
    thread->start();
}

void TransferFileWindow::onRejectTransferFileSignal() {
    QMessageBox::critical(this, "Fail:", "对方拒绝了你的传输申请");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
}

void TransferFileWindow::onFinish() {
    thread->quit();
    thread = nullptr;
    QMessageBox::information(this, "Success:", "发送完毕");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
    ui->progressBar->setValue(0);
}

void TransferFileWindow::onFail() {
    thread->quit();
    thread = nullptr;
    QMessageBox::critical(this, "Fail:", "发送失败");
    ui->state_label->setText("请打开文件");
    ui->open_file_button->setEnabled(true);
    ui->transfer_file_button->setEnabled(false);
    ui->progressBar->setValue(0);
}
