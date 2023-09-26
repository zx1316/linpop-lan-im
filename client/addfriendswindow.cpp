#include "addfriendswindow.h"
#include "ui_addfriendswindow.h"

AddFriendsWindow::AddFriendsWindow(const QString &selfName, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::AddFriendsWindow), launcher(launcher), selfName(selfName) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->name_search_line_edit->setPlaceholderText("请输入要添加的用户名");
    connect(ui->search_pushbutton,SIGNAL(clicked()),this,SLOT(onSearchPushButtonClicked()));
    connect(ui->secretButton, &QPushButton::clicked, this, [=] {
        launcher->directLaunch();
    });
}

AddFriendsWindow::~AddFriendsWindow() {
    emit windowClosed();
    delete ui;
}

void AddFriendsWindow::onSearchPushButtonClicked() {
    launcher->gachaLaunch();
    QString name = ui->name_search_line_edit->text();
    if (name == selfName) {
        QMessageBox::information(this,"查找完毕", "你不能添加自己");
    } else {
        ui->search_pushbutton->setEnabled(false);
        if (ui->type_combo_box->currentIndex() == 1) {
            emit addFriendRequestSignal("_" + name);
        } else {
            emit addFriendRequestSignal(name);
        }
    }
}

void AddFriendsWindow::onAddFriendSuccess() {
    ui->name_search_line_edit->clear();
    ui->search_pushbutton->setEnabled(true);
    QMessageBox::information(this,"查找完毕", "添加成功！");
}

void AddFriendsWindow::onAddFriendFail() {
    ui->search_pushbutton->setEnabled(true);
    QMessageBox::information(this,"查找完毕", "无此用户");
}

void AddFriendsWindow::onAddFriendAlready() {
    ui->search_pushbutton->setEnabled(true);
    qDebug() << "fuck you";
    QMessageBox::information(this,"查找完毕", "你已添加该好友");
}
