#include "groupmemberwindow.h"
#include "ui_groupmemberwindow.h"

GroupMemberWindow::GroupMemberWindow(QWidget *parent) : QWidget(parent), ui(new Ui::GroupMemberWindow) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->refreshButton->setDisabled(true);
    connect(ui->refreshButton, &QPushButton::clicked, this, &GroupMemberWindow::onRefreshClicked);
}

GroupMemberWindow::~GroupMemberWindow() {
    emit windowClosed();
    delete ui;
}

void GroupMemberWindow::onGroupMemberSignal(const QList<QString> &list) {
    auto listWidget = ui->listWidget;
    listWidget->clear();
    listWidget->addItems(list);
    ui->refreshButton->setDisabled(false);
}

void GroupMemberWindow::onRefreshClicked() {
    ui->refreshButton->setDisabled(true);
    emit GroupMemberRequestSignal();
}
