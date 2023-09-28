#include "friendinformation.h"
#include "ui_friendinformation.h"

/*
 * 函数名称：FriendInformation(构造函数)
 * 功能描述：初始化FriendInformation信息，并根据参数设置QLabel显示内容，连接信号和槽函数等
 * 作者:林方裕
 * 重制：朱炫曦
*/
FriendInformation::FriendInformation(const User& u, QWidget *parent) : QWidget(parent), ui(new Ui::FriendInformation) {
    ui->setupUi(this);
    //根据好友信息设置相应标签内容
    name = u.name;
    ui->username_label->setText(name[0] == '_' ? name.right(name.length() - 1) : name);
    ui->ip_label->setText(name[0] == '_' ? "聊天室" : (u.ip == "" ? "离线" : u.ip));
    ui->picture_label->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/cached_images/" + u.img));
    this->setStyleSheet((name[0] == '_' || u.ip != "") ? "#widget{border:1px solid black;background:Cyan}" : "#widget{border:1px solid black;background:gray}");

    //将信号和槽函数连接
    connect(ui->username_label, &DoubleClickedLabel::doubleClickedSignal, this, &FriendInformation::onDoubleClickedSignal);
    connect(ui->ip_label, &DoubleClickedLabel::doubleClickedSignal, this, &FriendInformation::onDoubleClickedSignal);
    connect(ui->picture_label, &DoubleClickedLabel::doubleClickedSignal, this, &FriendInformation::onDoubleClickedSignal);
    connect(ui->undefinedButton, SIGNAL(clicked()), this, SLOT(onUndefinedButtonClicked()));
}

FriendInformation::~FriendInformation() {
    delete ui;
}

void FriendInformation::onUndefinedButtonClicked() {
    emit undefinedButtonClickedSignal(this);
}

void FriendInformation::onDoubleClickedSignal() {
    emit doubleClickedSignal(this);
}
