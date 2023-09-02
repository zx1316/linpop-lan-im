#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget *parent, QString &ip, int *port) :
    QWidget(parent),
    ui(new Ui::ConfigWindow),
    ip(ip),
    port(port)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose, false);
    ui->ipEdit->setText(ip);
    ui->portEdit->setText(QString::number(*port));
    connect(ui->confirmButton, &QPushButton::clicked, this, &ConfigWindow::onConfirmClicked);
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}

void ConfigWindow::onConfirmClicked() {
    *port = ui->portEdit->text().toInt();
    ip = ui->ipEdit->text();
    close();
}
