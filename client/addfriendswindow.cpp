#include "addfriendswindow.h"
#include "ui_addfriendswindow.h"
#include "autostart.h"
#include <QDebug>

AddFriendsWindow::AddFriendsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriendsWindow)
{
    ui->setupUi(this);
    ui->name_search_line_edit->setPlaceholderText("请输入要添加的用户名");
    connect(ui->search_pushbutton,SIGNAL(clicked()),this,SLOT(onSearchPushButtonClicked()));
}

AddFriendsWindow::~AddFriendsWindow()
{
    qDebug("AddFriendWindow delete");
    delete ui;
}

void AddFriendsWindow::onSearchPushButtonClicked()
{
    gachaAutoStart(this);
    qDebug("AddFriendsWindow::addFriendRequest sent");
    QString username = ui->name_search_line_edit->text();
    bool type = ui->type_combo_box->currentIndex();
    emit addFriendRequestSignal(!type,username);
}

void AddFriendsWindow::onAddFriendFeedback(int feedback){
    qDebug("AddFriendWindow Add Friend Feedback");
    if(feedback==1){
        QMessageBox::information(this,"查找完毕", "添加成功！");
        ui->name_search_line_edit->clear();
    }else if(feedback==0){
        QMessageBox::information(this,"查找完毕", "无此用户");
    }else if(feedback==2){
        QMessageBox::information(this,"查找完毕", "你已添加该好友");
    }else{
        QMessageBox::information(this,"查找完毕", "你不能添加自己");
    }
}

void AddFriendsWindow::closeEvent(QCloseEvent *event){
    emit closeWindowSignal(this);
}

