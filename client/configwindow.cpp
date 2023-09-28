#include "configwindow.h"
#include "ui_configwindow.h"
#include <QMessageBox>
#include <QDir>

ConfigWindow::ConfigWindow(Network *network, QWidget *parent) : QWidget(parent), ui(new Ui::ConfigWindow), network(network) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    ui->ipEdit->setText(network->getIp());
    ui->portEdit->setText(QString::number(network->getPort()));
    connect(ui->confirmButton, &QPushButton::clicked, this, &ConfigWindow::onConfirmClicked);
    connect(ui->clearCacheButton, &QPushButton::clicked, this, &ConfigWindow::onClearCacheClicked);
}

ConfigWindow::~ConfigWindow() {
    delete ui;
}

void ConfigWindow::onConfirmClicked() {
    network->setIpAndPort(ui->ipEdit->text(), ui->portEdit->text().toUShort());
    close();
}

void ConfigWindow::onClearCacheClicked() {
    if (QMessageBox::question(this, "删除确认", "确定要清空图片缓存吗？", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        QDir dir(QCoreApplication::applicationDirPath() + "/cached_images");
        dir.removeRecursively();
        dir.cdUp();
        dir.mkdir("cached_images");
    }
}
