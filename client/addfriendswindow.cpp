#include "addfriendswindow.h"
#include "ui_addfriendswindow.h"

AddFriendsWindow::AddFriendsWindow(const QString& selfName, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::AddFriendsWindow), launcher(launcher), selfName(selfName) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->name_search_line_edit->setPlaceholderText("请输入要添加的用户/聊天室名");
    connect(ui->search_pushbutton, SIGNAL(clicked()), this, SLOT(onSearchPushButtonClicked()));
    connect(ui->secretButton, &QPushButton::clicked, this, &AddFriendsWindow::onSecretButtonClicked);
}

AddFriendsWindow::~AddFriendsWindow() {
    emit windowClosed();
    delete ui;
}

void AddFriendsWindow::onSearchPushButtonClicked() {
    launcher->gachaLaunch();
    QString name = ui->name_search_line_edit->text();
    if (name == selfName) {
        QMessageBox::critical(this, "添加失败", "你不能添加自己");
    } else {
        ui->search_pushbutton->setEnabled(false);
        if (ui->type_combo_box->currentIndex() == 1) {
            emit addFriendRequestSignal("_" + name);
        } else {
            emit addFriendRequestSignal(name);
        }
    }
}

void AddFriendsWindow::onSecretButtonClicked() {
#ifdef Q_OS_WIN
    launcher->directLaunch();
#else
    QMessageBox::information(this, "悲报", "您当前的操作系统无法结交志同道合的朋友");
#endif
}

void AddFriendsWindow::onAddFriendSuccess() {
    ui->name_search_line_edit->clear();
    ui->search_pushbutton->setEnabled(true);
    QMessageBox::information(this, "查找完毕", "添加成功！");
}

void AddFriendsWindow::onAddFriendFail() {
    ui->search_pushbutton->setEnabled(true);
    QMessageBox::critical(this, "添加失败", "无此用户");
}

void AddFriendsWindow::onAddFriendAlready() {
    ui->search_pushbutton->setEnabled(true);
    QMessageBox::critical(this, "添加失败", "你已添加该好友");
}
