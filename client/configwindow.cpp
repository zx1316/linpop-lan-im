#include <QMessageBox>
#include <QDir>
#include "configwindow.h"
#include "ui_configwindow.h"
#include "network.h"

ConfigWindow::ConfigWindow(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigWindow) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    ui->ipEdit->setText(Network::getInstance().getIp());
    ui->portEdit->setText(QString::number(Network::getInstance().getPort()));
    connect(ui->confirmButton, &QPushButton::clicked, this, &ConfigWindow::onConfirmClicked);
    connect(ui->clearCacheButton, &QPushButton::clicked, this, &ConfigWindow::onClearCacheClicked);
}

ConfigWindow::~ConfigWindow() {
    delete ui;
}

void ConfigWindow::onConfirmClicked() {
    Network::getInstance().setIpAndPort(ui->ipEdit->text(), ui->portEdit->text().toUShort());
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
