#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(Network *network, QWidget *parent) : QWidget(parent), ui(new Ui::ConfigWindow), network(network) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    ui->ipEdit->setText(network->getIp());
    ui->portEdit->setText(QString::number(network->getPort()));
    connect(ui->confirmButton, &QPushButton::clicked, this, &ConfigWindow::onConfirmClicked);
}

ConfigWindow::~ConfigWindow() {
    delete ui;
}

void ConfigWindow::onConfirmClicked() {
    network->setIpAndPort(ui->ipEdit->text(), ui->portEdit->text().toUShort());
    close();
}
