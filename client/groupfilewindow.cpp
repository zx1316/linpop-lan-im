#include "groupfilewindow.h"
#include "ui_groupfilewindow.h"
#include <QDebug>
#include <QDateTime>
#include <QFileDialog>
#include "indexwindow.h"

GroupFileWindow::GroupFileWindow(const QString& selfName, const QString& serverIp, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent),  ui(new Ui::GroupFileWindow), selfName(selfName), serverIp(serverIp), launcher(launcher) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    auto table = ui->tableWidget;
    connect(table, &QTableWidget::cellClicked, this, [=] (int row) {
        selectedRow = row;
    });
    connect(ui->deleteButton, &QPushButton::clicked, this, &GroupFileWindow::onDeleteClicked);
    connect(ui->downloadButton, &QPushButton::clicked, this, &GroupFileWindow::onDownloadClicked);
    connect(ui->uploadButton, &QPushButton::clicked, this, &GroupFileWindow::onUploadClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, &GroupFileWindow::onRefreshClicked);
    table->verticalHeader()->setHidden(true);
    table->setColumnCount(4);
    table->horizontalHeader()->setHighlightSections(false);         //点击表头时不对表头高亮
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    table->verticalHeader()->setDefaultSectionSize(35);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setHorizontalHeaderLabels({"文件名", "上传时间", "大小", "上传者"});
    table->setColumnWidth(0, 280);
    table->setColumnWidth(2, 80);
    table->setColumnWidth(3, 160);
    table->setColumnWidth(1, 120);
}

GroupFileWindow::~GroupFileWindow() {
    if (sendThread != nullptr) {
        sendThread->quit();
    }
    if (recvThread != nullptr) {
        recvThread->quit();
    }
    delete ui;
    emit windowClosed();
}

void GroupFileWindow::refreshFileList(const QList<GroupFile>& list) {
    launcher->gachaLaunch();
    auto table = ui->tableWidget;
    table->clearContents();
    table->setRowCount(list.size());
    int i = 0;
    for (auto& item : list) {
        table->setItem(i, 0, new QTableWidgetItem(item.name));
        table->setItem(i, 1, new QTableWidgetItem(QDateTime::fromMSecsSinceEpoch(item.timestamp).toString("yyyy-MM-dd")));
        table->setItem(i, 2, new QTableWidgetItem(IndexWindow::fileSizeFormatter(item.size)));
        table->setItem(i, 3, new QTableWidgetItem(item.uploader));
        i++;    // bug fixed?
    }
    selectedRow = -1;
    ui->refreshButton->setDisabled(false);
}

void GroupFileWindow::onRefreshClicked() {
    launcher->gachaLaunch();
    ui->refreshButton->setDisabled(true);
    emit groupFileQuerySignal();
}

void GroupFileWindow::onUploadClicked() {
    launcher->gachaLaunch();
    QString path = QFileDialog::getOpenFileName(this, "打开文件", "../");
    if (path != "") {
        ui->downloadButton->setDisabled(true);
        ui->uploadButton->setDisabled(true);
        ui->deleteButton->setDisabled(true);
        sendThread = new ServerSendThread(path, serverIp);
        auto timer = new QTimer;
        timer->setInterval(30000);
        QFileInfo info(path);
        connect(sendThread, &ServerSendThread::portAvailable, this, [=](quint16 port) {
            emit groupFileUploadSignal(info.fileName(), info.size(), port);
        });
        connect(sendThread, &ServerSendThread::fail, this, &GroupFileWindow::onUploadFail);
        connect(sendThread, &ServerSendThread::success, this, [=] {
            sendThread->quit();
            sendThread = nullptr;
            QMessageBox::information(this,"Success:","文件上传成功");
            auto table = ui->tableWidget;
            table->insertRow(0);
            table->setItem(0, 0, new QTableWidgetItem(info.fileName()));
            table->setItem(0, 1, new QTableWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd")));
            table->setItem(0, 2, new QTableWidgetItem(IndexWindow::fileSizeFormatter(info.size())));
            table->setItem(0, 3, new QTableWidgetItem(selfName));
            ui->downloadButton->setDisabled(false);
            ui->uploadButton->setDisabled(false);
            ui->deleteButton->setDisabled(false);
            ui->progressBar->setValue(0);
        });
        connect(sendThread, &ServerSendThread::progress, this, [=](qint8 progress) {
            ui->progressBar->setValue(progress);
            timer->start();
        });
        connect(sendThread, &ServerSendThread::finished, sendThread, &ServerSendThread::deleteLater);
        connect(sendThread, &ServerSendThread::finished, timer, &QTimer::deleteLater);
        connect(timer, &QTimer::timeout, this, &GroupFileWindow::onUploadFail);
        timer->start();
        sendThread->start();
    }
}

void GroupFileWindow::onDownloadClicked() {
    launcher->gachaLaunch();
    if (selectedRow != -1) {
        QString path = QFileDialog::getSaveFileName(this, "保存文件", "../" + ui->tableWidget->selectedItems()[0]->text());
        if (path != "") {
            ui->downloadButton->setDisabled(true);
            ui->uploadButton->setDisabled(true);
            ui->deleteButton->setDisabled(true);
            recvThread = new ServerRecvThread(path, serverIp);
            auto timer = new QTimer;
            timer->setInterval(30000);
            connect(recvThread, &ServerRecvThread::portAvailable, this, [=](quint16 port) {
                emit groupFileDownloadSignal(QFileInfo(path).fileName(), port);
            });
            connect(recvThread, &ServerRecvThread::fail, this, &GroupFileWindow::onDownloadFail);
            connect(recvThread, &ServerRecvThread::success, this, &GroupFileWindow::onDownloadSuccess);
            connect(recvThread, &ServerRecvThread::progress, this, [=](qint8 progress) {
                ui->progressBar->setValue(progress);
                timer->start();
            });
            connect(recvThread, &ServerRecvThread::finished, recvThread, &ServerRecvThread::deleteLater);
            connect(recvThread, &ServerRecvThread::finished, timer, &QTimer::deleteLater);
            connect(timer, &QTimer::timeout, this, &GroupFileWindow::onDownloadFail);
            timer->start();
            recvThread->start();
        }
    }
}

void GroupFileWindow::onDeleteClicked() {
    launcher->gachaLaunch();
    auto table = ui->tableWidget;
    if (selectedRow != -1 && table->selectedItems()[3]->text() == selfName) {
        if (QMessageBox::question(this, "警告", "您确定要删除该文件吗？", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
            emit groupFileDeleteSignal(table->selectedItems()[0]->text());
            table->removeRow(selectedRow);
        }
    } else {
        QMessageBox::critical(this, "删除失败", "您不能删除不是您上传的文件！");
    }
}

void GroupFileWindow::onUploadFail() {
    sendThread->quit();
    sendThread = nullptr;
    QMessageBox::critical(this,"Fail:","文件上传失败");
    ui->downloadButton->setDisabled(false);
    ui->uploadButton->setDisabled(false);
    ui->deleteButton->setDisabled(false);
    ui->progressBar->setValue(0);
}

void GroupFileWindow::onDownloadSuccess() {
    recvThread->quit();
    recvThread = nullptr;
    QMessageBox::information(this,"Success:","文件下载成功");
    ui->downloadButton->setDisabled(false);
    ui->uploadButton->setDisabled(false);
    ui->deleteButton->setDisabled(false);
    ui->progressBar->setValue(0);
}

void GroupFileWindow::onDownloadFail() {
    recvThread->quit();
    recvThread = nullptr;
    QMessageBox::critical(this,"Fail:","文件下载失败");
    ui->downloadButton->setDisabled(false);
    ui->uploadButton->setDisabled(false);
    ui->deleteButton->setDisabled(false);
    ui->progressBar->setValue(0);
}
