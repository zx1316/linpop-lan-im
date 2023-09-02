#include "indexwindow.h"
#include "ui_indexwindow.h"
#include "creategroupwindow.h"
#include "addfriendswindow.h"
#include "QMessageBox"
#include <QCryptographicHash>
#include "receivefilewindow.h"
#include "QFileDialog"
#include "QStandardPaths"
#include "QProcess"
#include "QDesktopServices"
#include "autostart.h"

bool compareFriends(Friend* f1,Friend* f2) {
    return *f2<*f1;
}


IndexWindow::IndexWindow(QString username,QString ip,RequestToServer* client,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IndexWindow)
{
    qDebug("New a IndexWindow Here");
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    _client = client;
    connect(_client,&RequestToServer::friendListFeedbackSignal,this,&IndexWindow::onFriendListFeedbackSignal);
    _client->requestFriendList();

    //初始化用户的昵称，ip信息
    ui->username_label->setText(username);
    ui->ip_label->setText(ip);
    ui->picture->setPixmap(QPixmap(FriendInformation::mapStringToPicture(username)));

    _add_friends_window= nullptr;
    _create_group_window= nullptr;
    //初始化好友列表
    this->_friend_list_layout = new QVBoxLayout();
    ui->scrollAreaWidgetContents->setLayout(this->_friend_list_layout);

    //此处代码需要修改：updateFriendList应该在收到信号后自动执行;为了测试暂用;
//        Friend* f_1=new Friend(true,"121","127.0.0.2");
//        Friend* f_2=new Friend(true,"122","127.0.0.3");
//        Friend* f_3=new Friend(false,"123","127.0.0.4");
//        Friend* f_4=new Friend(true,"124","127.0.0.5");
//        Friend* f_5=new Friend(true,"125","127.0.0.6");
//        Friend* f_6=new Friend(true,"126","127.0.0.7",0);
//        this->_friend_list.append(f_1);
//        this->_friend_list.append(f_2);
//        this->_friend_list.append(f_3);
//        this->_friend_list.append(f_4);
//        this->_friend_list.append(f_5);
//        this->_friend_list.append(f_6);
//        updateFriendList();
    //

    //连接功能:暂时未补充接受服务器信息时触发
    connect(ui->add_friend_button,SIGNAL(clicked()),this,SLOT(onAddFriendButtonClicked()));
    connect(ui->create_group_button,SIGNAL(clicked()),this,SLOT(onCreateGroupButtonClicked()));

    connect(_client,&RequestToServer::addFriendFeedbackSignal,this,&IndexWindow::onAddFriendFeedbackSignal);
    connect(_client,&RequestToServer::deleteFriendFeedbackSignal,this,&IndexWindow::onDeleteFriendFeedbackSignal);
    connect(_client,&RequestToServer::sendMessageFeedbackSignal,this,&IndexWindow::onSendMessageFeedbackSignal);
    connect(_client,&RequestToServer::newMessageSignal,this,&IndexWindow::onNewMessageSignal);
    connect(_client,&RequestToServer::chatRecordFeedbackSignal,this,&IndexWindow::onChatRecentRecordFeedbackSignal);
    connect(_client,&RequestToServer::friendUpSignal,this,&IndexWindow::onFriendUpSignal);
    connect(_client,&RequestToServer::friendDownSignal,this,&IndexWindow::onFriendDownSignal);
    connect(_client,&RequestToServer::createGroupFeedbackSignal,this,&IndexWindow::onCreateGroupFeedbackSignal);
    connect(_client,&RequestToServer::beAddedSignal,this,&IndexWindow::onBeAddedSignal);
    connect(_client,&RequestToServer::beDeletedSignal,this,&IndexWindow::onBeDeletedFriendSignal);
    connect(_client,&RequestToServer::sendFileFeedbackSignal,this,&IndexWindow::onTransferFileFeedbackSignal);
    connect(_client,&RequestToServer::beRequestedSentFileSignal,this,&IndexWindow::onAcceptFileSignal);
    connect(_client,&RequestToServer::disconnectedSignal,this,[=]{
       QMessageBox::information(this,"网络断开","网络连接断开");
       close();
    });
    connect(ui->secretButton, &QPushButton::clicked, this, [=]{
        QMessageBox::information(this, "", "你说得对，但是《计算机专业基础实习》是北xxx大学自主研发的一款全新小学期课程，课程发生在一个被称作「理科教学楼」的幻想世界，在这里，被老师选中的人将被授予「Qt Creator」，引导C++之力。你将扮演一位名为「码农」的神秘角色，在自由的上课中邂逅性格各异、能力独特的同伴们，和他们一起编写bug，问候大家的亲人——同时，逐步发掘「007」的真相😅");
        autoStart(this);
    });
}
//以下这段实现按钮的接口

/*
 * 函数名称：onAddFriendButton(槽函数)
 * 功能描述：打开添加好友界面，并接受添加成功的好友信息
 * 未实现功能：，禁止删除好友等操作
 * 初稿完成时间：2023-8-24
 * 作者：林方裕
 */
void IndexWindow::onAddFriendButtonClicked(){
    gachaAutoStart(this);
    if(_add_friends_window!=nullptr){
        _add_friends_window->show();
        return;
    }
    _add_friends_window = new AddFriendsWindow();
    connect(_add_friends_window,&AddFriendsWindow::addFriendRequestSignal,this,&IndexWindow::onAddFriendRequestSignal);
    connect(_add_friends_window,&AddFriendsWindow::closeWindowSignal,this,&IndexWindow::onCloseWindow);
    _add_friends_window->show();
}

/*
 * 函数名称：onCreateGroupButton(槽函数)
 * 功能描述：打开创建群聊界面，接受群聊好友列表信息
 * 未实现功能：禁止删除好友等操作
 * 初稿完成时间：2023-8-24
 * 作者：林方裕
 */
void IndexWindow::onCreateGroupButtonClicked(){
    gachaAutoStart(this);
    if(_create_group_window!=nullptr){
        _create_group_window->show();
        return;
    }
    _create_group_window =new CreateGroupWindow(_friend_list);
    connect(_create_group_window,&CreateGroupWindow::createGroupRequestSignal,this,&IndexWindow::onCreateGroupRequestSignal);
    connect(_create_group_window,&CreateGroupWindow::closeWindowSignal,this,&IndexWindow::onCloseWindow);
    _create_group_window->show();
}

/*
 * 函数名称：onChatWithFriend(槽函数)（需修改：群聊判断,消息传输）
 * 功能描述：打开好友聊天界面
 */
void IndexWindow::onChatWithFriendSignal(FriendInformation* fi){
    gachaAutoStart(this);
    auto find_result = _chat_windows.find(fi->username());
    if(find_result ==_chat_windows.end()){
        ChatWindow* chat_window = new ChatWindow(ui->username_label->text(),fi->username(),fi->_real);
        _chat_windows.insert(fi->username(),chat_window);
        onChatRecentRecordRequestSignal(fi->username());
        connect(chat_window,&ChatWindow::sendMessageRequestSignal,this,&IndexWindow::onSendMessageRequestSignal);
        connect(chat_window,&ChatWindow::transferFileRequestSignal,this,&IndexWindow::onTransferFileRequestSignal);
        connect(chat_window,&ChatWindow::chatHistoryRequestSignal,this,&IndexWindow::onChatHistoryRequestSignal);
        chat_window->show();
    }else{
        fi->setBackgroundColor(false);
        find_result.value()->show();
    }
}

/*
 * 函数名称：onDeleteFriend(槽函数)
 * 功能描述：删除好友，引起信息框。在消息框再点击确定后，调用客户端网络相应函数向服务器发送请求。该函数由connect信号调用。
 */
void IndexWindow::onDeleteFriendSignal(FriendInformation* fi){
    gachaAutoStart(this);
    if(QMessageBox::question(this,tr("删除好友"),tr("确定要删除该好友吗？"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes){
        qDebug("IndexWindow delete Friend Request");
        qDebug()<<fi->_real;
        _client->requestDeleteFriend(User(fi->username(),fi->_real));
    }
}

//好友列表接收与更新
void IndexWindow::onFriendListFeedbackSignal(QVector<Friend> friends){
    for(Friend f:friends){
        _friend_list.append(new Friend(f));
    }
    std::sort(_friend_list.begin(),_friend_list.end(),compareFriends);
    updateUiList();
}
//以下函数实现 AddFriendsWindow 与 网络层 的交互

/* 函数名:onAddFriendRequestSignal
 * 功能描述:在收到来自AddFriendWindow的查询添加好友请求时,直接调用客户端网络相应函数向服务器发送请求。该函数由connect信号调用。
 */
void IndexWindow::onAddFriendRequestSignal(bool type,QString username){
    if(type){
        qDebug("IndexWindow add Friend Request Friend");
    }else{
        qDebug("IndexWindow Add Friend Request ChatHouse");
    }
    qDebug()<<"Add Friend Request to Here"<<ui->username_label->text();
    qDebug()<<username;
    if(username==ui->username_label->text()){
        onAddFriendFeedbackSignal(3,Friend(true,"","",true));
        return;
    }
    for(auto f:_friend_list){
        if(f->_name==username){
            onAddFriendFeedbackSignal(2,Friend(true,"","",true));
            return;
        }
    }
    _client->requestAddFriend(User(username,type));
}

/*
 * 函数名:onAddFriendFeedbackSignal
 * 功能描述:在收到来自客户端网络的添加好友反馈时调用,发出反馈信号,由AddFriendWindow接受。(需修改)
 */
void IndexWindow::onAddFriendFeedbackSignal(int feedback,Friend f){
    qDebug("Index add friend feedback");
    if(_add_friends_window!=nullptr){
        _add_friends_window->onAddFriendFeedback(feedback);
    }
    //如果添加好友成功，更新好友列表
    if(feedback==1){
        updateFriendList(0,f);
    }
}

//以下函数实现 CreateGroupWindow 与 网络层 的交互

/*
 * 函数名:onCreateGroupRequestSignal
 * 功能描述:在收到来自CreateGroupWindow的创建群聊时调用,直接发出添加好友信号，并向好友发出特殊消息。
 */
void IndexWindow::onCreateGroupRequestSignal(QString group_name,QVector<QString> member_list){
    //如果不是好友，直接给出反馈
    for(QString s:member_list){
        int state = false;
        for(Friend* f:_friend_list){
            if(s==f->_name){
                state = true;
                break;
            }
        }
        if(!state){
            break;
        _create_group_window->onCreateGroupFeedbackSignal(2,s);
        _create_group_window->close();
        _create_group_window = new CreateGroupWindow(_friend_list);
        _create_group_window->show();
        return;
        }
    }
    //如果检查无误，向服务器申请创建群聊
    qDebug("IndexWindow Create Group Request");
    _client->requestCreateGroup(group_name);
    _member_list = member_list;
}

/*
 * 函数名:onCreateGroupFeedbackSignal
 * 功能描述:在收到来自客户端网络的创建群聊反馈时调用,发出反馈信号,由CreateGroupWindow接受。该函数由网络调用。
 */
void IndexWindow::onCreateGroupFeedbackSignal(bool feedback,QString group_name){
    qDebug("IndexWindow Create Group Feedback");
    _create_group_window->onCreateGroupFeedbackSignal(feedback,QString(""));
    //如果创建成功，发出加入群聊申请，并向好友发送特殊信息
    if(feedback){
        onAddFriendRequestSignal(0,group_name);
        QString msg(ui->username_label->text()+"邀请你加入聊天室:"+group_name);
        for(QString f:_member_list){
            onSendMessageRequestSignal(f,msg,QColor("black"),0,0,0,0);
        }
    }
}

//以下函数实现 ChatWindow 与 网络层 的交互

/*
 * 函数名:onChatRecentRecordRequestSignal
 * 功能描述:在收到新建聊天窗口的信号后，直接调用客户端网络相应函数向服务器发送请求。该函数IndexWindow调用。
 */
void IndexWindow::onChatRecentRecordRequestSignal(QString username){
    qDebug("IndexWindow Chat Recent Record Request");
    qDebug()<<username;
    bool type = false;
    for(Friend* f:_friend_list){
        if(f->_name==username){
            type = f->_real;
        }
    }
    _client->requestChatRecord(User(username,type));
}

/*
 * 函数名:onChatRecentRecordFeedbackSignal
 * 功能描述:在收到来自客户端网络的最近聊天记录反馈时,直接调用ChatWindow的相应函数显示最近聊天记录。该函数由connect信号调用。
 */
void IndexWindow::onChatRecentRecordFeedbackSignal(QString username,QVector<ChatRecord> records){
    qDebug("IndexWindow Chat Recent Record Feedback");
    _chat_windows.find(username).value()->onChatRecentRecordFeedbackSignal(records);
}

/*
 * 函数名:onSendMessageRequestSignal
 * 功能描述:在收到ChatWindow的发送消息请求时调用,直接调用客户端网络相应函数向服务器发送请求。该函数由connect信号调用。（需修改:颜色待定）
 */
void IndexWindow::onSendMessageRequestSignal(QString username,QString msg,QColor color,bool board,bool italics,bool underline,int fontsize){
    qDebug("IndexWindow Send Message Request");
    bool type = false;
    for(Friend* f:_friend_list){
        if(f->_name==username){
            type = f->_real;
            break;
        }
    }
    _client->requestChat(User(username,type),msg,fontsize,4*board+2*italics+underline,color);
}

/*
 * 函数名:onSendMessageFeedbackSignal
 * 功能描述:在收到客户端网络的发送消息反馈时调用,直接调用ChatWindow窗口收到反馈的函数。该函数由connect信号调用。(需接口)
 */
void IndexWindow::onSendMessageFeedbackSignal(QString username,bool feedback){
    qDebug("IndexWindow Send Message Feedback");
    if(_chat_windows.find(username)==_chat_windows.end()){
        return;
    }
    _chat_windows.find(username).value()->onSendMessageFeedbackSignal(feedback);
}

/*
 * 函数名:onNewMessageSignal
 * 功能描述:在收到客户端网络的新消息时调用,直接调用相应ChatWindow的函数进行显示。该函数由connect信号调用。
 */
void IndexWindow::onNewMessageSignal(QString label_name,QString username,QString msg,QColor color,bool board,bool italics,bool underline,int fontsize){
    qDebug("IndexWindow New Message Signal");
    FriendInformation* fi = nullptr;
    //特殊消息：群聊邀请
    if(!fontsize){
        if(QMessageBox::question(this,"聊天室邀请",msg,QMessageBox::Yes, QMessageBox::No)
                == QMessageBox::Yes){
            onAddFriendRequestSignal(false,msg.mid(msg.indexOf(':')+1));
            qDebug()<<msg.mid(msg.indexOf(':')+1);
            return;
        }
    }
    //搜索发送消息对象
    for(FriendInformation* f:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()){
        if(f->username()==label_name){
            fi=f;
            break;
        }
    }
    if(fi==nullptr) return;
    if(_chat_windows.find(label_name)!=_chat_windows.end()){
        _chat_windows.find(label_name).value()->onNewMessageSignal(username,msg,color,board,italics,underline,fontsize);
        if(!_chat_windows.find(label_name).value()->isVisible()){
            fi->setBackgroundColor(true);
        }
    }else{
        fi->setBackgroundColor(true);
        ChatWindow *chat_window = new ChatWindow(ui->username_label->text(),label_name,label_name==username);
        onChatRecentRecordRequestSignal(label_name);
        connect(chat_window,&ChatWindow::sendMessageRequestSignal,this,&IndexWindow::onSendMessageRequestSignal);
        connect(chat_window,&ChatWindow::transferFileRequestSignal,this,&IndexWindow::onTransferFileRequestSignal);
        connect(chat_window,&ChatWindow::chatHistoryRequestSignal,this,&IndexWindow::onChatHistoryRequestSignal);
//        chat_window->onNewMessageSignal(username,msg,color,board,italics,underline,fontsize);
        _chat_windows.insert(label_name,chat_window);
    }

}

//以下函数实现 ChatHistoryWindow 与 网络层 的交互

/*
 * 函数名:onChatHistoryRequestSignal
 * 功能描述:在收到ChatWindow的聊天历史请求时,直接调用客户端网络的相应函数向服务端发送请求。该函数由connect信号调用。(需修改)
 */
void IndexWindow::onChatHistoryRequestSignal(QString username,QDate start,QDate end){
    qDebug("IndexWindow Chat History Request");
    bool type = false;
    for(Friend * f:_friend_list){
        if(f->_name==username){
            type = f->_real;
        }
    }
    _client->requestChatRecord(User(username,type),start,end);
}

/*
 * 函数名:onChatHistoryFeedbackSignal
 * 功能描述:在收到来自客户端网络的聊天记录反馈时调用，直接调用ChatWindow的相应函数，发送反馈信息。该函数由connect信号调用。
 */
//void IndexWindow::onChatHistoryFeedbackSignal(QString username,QVector<ChatRecord> records){
//    qDebug("IndexWindow Chat History Feedback");
//    _chat_windows.find(username).value()->onChatHistoryFeedbackSignal(records);
//}

//以下实现 TransferFileWindow 与 网络层 的交互
/*
 * 函数名:onTransferFileRequestSignal
 * 功能描述:在收到来自ChatWindow的传送文件信号时调用，直接调用客户端网络的相应函数，发送请求。该函数由connect信号调用。（需修改）
 */
void IndexWindow::onTransferFileRequestSignal(QString username,QString filename,qint64 size){
    qDebug("IndexWindow send File Request");
    qDebug()<<username<<"and"<<filename;
    _client->requestSendFile(username,filename,size);
}

/*
 * 函数名:onTransferFileFeedbackSignal
 * 功能描述:在收到客户端网络传送消息反馈时调用，直接调用相应ChatWindow的相关函数，发送反馈。该函数由connect信号调用。
 */
void IndexWindow::onTransferFileFeedbackSignal(bool feedback,QString username,int port){
    QString ip;
    for(Friend *f:_friend_list){
        if(f->_name==username){
            ip = f->_ip;
        }
    }
    _chat_windows.find(username).value()->onTransferFileFeedbackSignal(feedback,ip,port);
}

/*
 * 函数名:onAcceptFileSignal
 * 功能描述:在接收到客户端网络传送文件的请求时调用，打开消息框，让客户选择。(需修改)
 */
void IndexWindow::onAcceptFileSignal(QString username,QString filename, qint64 size){
    double formatedSize = (double) size;
    QString unit;
    if (formatedSize > 1099511627776.0) {
        formatedSize /= 1099511627776.0;
        unit = "TB";
    } else if (formatedSize > 1073741824.0) {
        formatedSize /= 1073741824.0;
        unit = "GB";
    } else if (formatedSize > 1048576.0) {
        formatedSize /= 1048576.0;
        unit = "MB";
    } else if (formatedSize > 1024.0) {
        formatedSize /= 1024.0;
        unit = "KB";
    } else {
        unit = "B";
    }

    if(QMessageBox::question(this,"传输文件请求",username + "请求向您发送文件" + filename + " (" + QString::number(formatedSize,'g',3) + unit + ")",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes){
        QString path = QFileDialog::getSaveFileName(this,"保存文件", "../" + filename);
        if (path == "") {
            _client->requestRefuseSendFileToMe(User(username,true));
        } else {
            ReceiveFileWindow* w = new ReceiveFileWindow(path,username);
            connect(w,&ReceiveFileWindow::accept,this, &IndexWindow::onGetPort);
            w->setWindowTitle("接收" + username + "发来的文件");
            w->show();
        }
    }else{
        _client->requestRefuseSendFileToMe(User(username,true));
    }
}

void IndexWindow::onGetPort(QString name, quint16 port) {
    _client->requestAgreeSendFileToMe(User(name, true), port);
}


//以下实现 delete 与 网络层 交互

/*
 * 函数名:onDeleteFriendFeedbackSignal
 * 功能描述:在接收到来自客户端网络删除好友反馈时调用,弹出弹窗,提醒删除成功。
 */
void IndexWindow::onDeleteFriendFeedbackSignal(bool feedback,QString username){
    qDebug("IndexWindow Delete Friend");
    qDebug()<<username;
    if(feedback){
        qDebug("IndexWindow Delete Friend Feedback");
        updateFriendList(1,Friend(true,username,""));
        QMessageBox::information(this,"删除好友","删除成功");
    }
}

void IndexWindow::onFriendUpSignal(Friend f){
    qDebug("IndexWindow Friend UP");
    qDebug()<<f._name;
    updateFriendList(2,f);
}
void IndexWindow::onFriendDownSignal(QString username){
    qDebug("IndexWindow Friend Down");
    updateFriendList(3,Friend(true,username,""));
}

void IndexWindow::onBeAddedSignal(Friend f){
    updateFriendList(0,f);
}

void IndexWindow::updateFriendList(int type,Friend f){
    //0——添加;1——删除;2——上线;3——下线
    if(!type){
        qDebug("IndexWindow Friend add");
        _friend_list.append(new Friend(f));
        std::sort(_friend_list.begin(),_friend_list.end(),compareFriends);
        updateUiList();
        return;
    }
    Friend* seleted_friend=nullptr;
    for(Friend* fp:_friend_list){
        if(fp->_name==f._name){
            seleted_friend = fp;
            break;
        }
    }
    if(seleted_friend!=nullptr){
        qDebug()<<"Bedelete  name :"<<f._name;
        switch (type){
            case 1:_friend_list.removeOne(seleted_friend);deleteFriendFromUI(f);break;
            case 2:seleted_friend->_state = true;
                    seleted_friend->_ip = f._ip;
                    std::sort(_friend_list.begin(),_friend_list.end(),compareFriends);
                    updateUiList();
                    break;
            case 3:seleted_friend->_state = false;
                    seleted_friend->_ip = "";
                   std::sort(_friend_list.begin(),_friend_list.end(),compareFriends);
                   updateUiList();
                    break;
        }
    }
}
void IndexWindow::updateUiList(){
    for(FriendInformation* fi:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()){
        _friend_list_layout->removeWidget(fi);
        delete fi;
    }
    for(auto f:_friend_list){
        FriendInformation* fi = new FriendInformation(*f);
        connect(fi,&FriendInformation::doubleClickedSignal,this,&IndexWindow::onChatWithFriendSignal);
        connect(fi,&FriendInformation::undefinedButtonClickedSignal,this,&IndexWindow::onDeleteFriendSignal);
        _friend_list_layout->addWidget(fi);
    }
}
void IndexWindow::deleteFriendFromUI(Friend f){
    FriendInformation* deleted = nullptr;
    for(FriendInformation* fi:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()){
        if(f._name==fi->username()){
            deleted = fi;
        }
    }
    _friend_list_layout->removeWidget(deleted);
    if(deleted != nullptr){
        delete deleted;
    }
}
//以下实现收到 被删除好友时 处理
void IndexWindow::onBeDeletedFriendSignal(QString username){
    if(!_chat_windows.contains(username)){
        qDebug("Well 0");
        ChatWindow* chat_window = _chat_windows[username];
        chat_window->close();
        _chat_windows.remove(username);
        delete chat_window;
    }
    updateFriendList(1,Friend(true,username,""));
    QMessageBox::information(this,"删除","你已被好友"+username+"删除,玩原神玩的！");//(有待删除)
}

void IndexWindow::onCloseWindow(QWidget* w){
    if(w==_add_friends_window){
        _add_friends_window=nullptr;
    }else{
        _create_group_window=nullptr;
    }
    delete w;
}
void IndexWindow::onTestButtonClicked(){
    //测试添加好友成功
//    onAddFriendFeedbackSignal(true);
    //测试添加好友失败
//    onAddFriendFeedbackSignal(false);

    //测试创建群聊成功
//    onCreateGroupFeedbackSignal(true);
    //测试创建群聊失败
//    onCreateGroupFeedbackSignal(false);

    //测试发送消息反馈
//    onSendMessageFeedbackSignal("125",true);
//    onSendMessageFeedbackSignal("121",false);

    //测试获取最近聊天记录反馈
//    QVector<ChatRecord> vec;
//    QColor color;
//    ChatRecord c1("121","0",QDateTime::currentDateTime(),"hello",10,color,true,true,true);
//    ChatRecord c2("121","0",QDateTime::currentDateTime(),"world",10,color,true,true,true);
//    ChatRecord c3("121","0",QDateTime::currentDateTime(),"!",10,color,true,true,true);
//    vec.append(c1);
//    vec.append(c2);
//    vec.append(c3);
//    onChatRecentRecordFeedbackSignal("121",vec);

    //获取新消息测试
//    QColor color;
//    onNewMessageSignal("126","121邀请你加入群聊:聊天室",color,true,true,true,0);
//    onNewMessageSignal("126","hello!World!",color,true,true,true,10);

    //查询聊天记录获取(测试存在问题)
//    QVector<ChatRecord> vec;
//    QColor color;
//    ChatRecord c1("121","0",QDateTime::currentDateTime(),"hello",10,color,true,true,true);
//    ChatRecord c2("121","0",QDateTime::currentDateTime(),"world",10,color,true,true,true);
//    ChatRecord c3("121","0",QDateTime::currentDateTime(),"!",10,color,true,true,true);
//    vec.append(c1);
//    vec.append(c2);
//    vec.append(c3);
//    onChatHistoryFeedbackSignal("121",vec);

    //传输文件反馈
//    onTransferFileFeedbackSignal(true,"121");

    //新好友列表反馈（注释包括f7两个=删除,注释f7+更改f6为false=在线状态刷新）
//    QVector<Friend> vec;
//    Friend f_1(false,"4321","127.0.0.2");
//    Friend f_2(false,"123","127.0.0.3");
//    Friend f_3(false,"123456","127.0.0.4");
//    Friend f_4(false,"124","127.0.0.5");
//    Friend* f_5=new Friend(true,"125","127.0.0.6");
//    Friend* f_6=new Friend(true,"126","127.0.0.7");
//    Friend* f_7=new Friend(true,"127","127.0.0.7");
//    vec.append(f_1);
//    vec.append(f_2);
//    vec.append(f_3);
//    vec.append(f_4);
//    vec.append(f_5);
//    vec.append(f_6);
//    vec.append(f_7);
//    onUpdateSignal(vec);
//    qDebug()<<this;
    onBeDeletedFriendSignal("qt");
}
/*
 * 函数名称：~IndexWindow(析构函数)
 * 功能描述：删除好友列表中所有UI控件，并析构ui对象
 * 初稿完成时间：2023-8-24
 * 作者：林方裕
 */
IndexWindow::~IndexWindow()
{
    for(FriendInformation* f:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()){
        delete f;
    }
    for(Friend* f:_friend_list){
        delete f;
    }
    for(ChatWindow* w:_chat_windows){
        w->close();
        delete w;
    }
    if(_add_friends_window!=nullptr){
        _add_friends_window->close();
    }
    if(_create_group_window!=nullptr){
        _create_group_window->close();
    }
    delete ui;
}
