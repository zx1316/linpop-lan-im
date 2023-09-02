#include "creategroupwindow.h"
#include "ui_creategroupwindow.h"
#include "QLayout"
#include "QDebug"
#include "QMessageBox"

CreateGroupWindow::CreateGroupWindow(QVector<Friend*> friend_list,QWidget *parent) :
    QWidget(parent),ui(new Ui::CreateGroupWindow)
{
    ui->setupUi(this);
    //为显示区域添加布局
    this->not_selected_layout = new QVBoxLayout();
    ui->not_selected_widget->setLayout(this->not_selected_layout);
    this->selected_layout = new QVBoxLayout();
    ui->selected_widget->setLayout(this->selected_layout);

    QVector<Friend*> online_friend_list;
    for(Friend* f:friend_list){
        if(f->_state && f->_real){
            online_friend_list.append(f);
        }
    }

    //初始化好友列表并显示
    init(online_friend_list);

    //连接按钮
    connect(ui->create_group_button,&QPushButton::clicked,this,&CreateGroupWindow::onCreateGroupButtonClicked);
    connect(ui->quit_button,SIGNAL(clicked()),this,SLOT(close()));
}

void CreateGroupWindow::init(QVector<Friend*> friend_list){
    //根据not_seleted_friend的内容添加显示控件
    for(Friend* f:friend_list){
        FriendInformation* uf = new FriendInformation(*f);
        uf->undefined_button->setText("加入");
        connect(uf,&FriendInformation::undefinedButtonClickedSignal,this,&CreateGroupWindow::change);
        this->not_selected_layout->addWidget(uf);
    }
}

void clearLayout(QWidget* widget){
    //清空layout中的
    for(auto i:widget->findChildren<FriendInformation*>()){
        delete i;
    }
}


void CreateGroupWindow::onCreateGroupButtonClicked(){
    qDebug("Click Create Group");
    QVector<QString> friend_usernames;
    for(FriendInformation* fi:ui->selected_widget->findChildren<FriendInformation*>()){
        friend_usernames.append(fi->username());
    }
    QString group_name = ui->group_name->text();
    if(group_name.startsWith('_')){
        QMessageBox::information(this,"创建失败","聊天室名称不能以下划线开头");
    }else if(group_name.toUtf8().size()>30){
        QMessageBox::information(this,"创建失败","聊天室名称过长");
    }else if(group_name.isEmpty()){
        QMessageBox::information(this,"创建失败","聊天室名称不能为空");
    }else{
        qDebug("CreateGroupWindow Create Group Request");
        emit createGroupRequestSignal(group_name,friend_usernames);
    }
}

void CreateGroupWindow::onCreateGroupFeedbackSignal(int feedback,QString name){
    qDebug("CreateGroupWindow Create Group Feedback");
    if(feedback==1){
        QMessageBox::information(this,"创建成功","创建聊天室成功");
        close();
    }else if(!feedback){
        QMessageBox::information(this,"创建失败","该聊天室已存在");
    }else{
        QMessageBox::information(this,"创建失败",name+"不是您的好友");
    }
}

void CreateGroupWindow::change(FriendInformation* uf){
    //判断好友控件在哪一个layout对象中，移至另一layout对象并更改按键名称
    if(this->not_selected_layout->indexOf(uf)!=-1){
        this->not_selected_layout->removeWidget(uf);
        this->selected_layout->addWidget(uf);
        uf->undefined_button->setText("移除");
    }else{
        this->selected_layout->removeWidget(uf);
        this->not_selected_layout->addWidget(uf);
        uf->undefined_button->setText("加入");
    }
}

CreateGroupWindow::~CreateGroupWindow()
{
    qDebug("CreateGroupWindow delete");
    clearLayout(ui->selected_widget);
    clearLayout(ui->not_selected_widget);
    delete not_selected_layout;
    delete selected_layout;
    delete ui;
}

void CreateGroupWindow::closeEvent(QCloseEvent *event){
    emit closeWindowSignal(this);
}
