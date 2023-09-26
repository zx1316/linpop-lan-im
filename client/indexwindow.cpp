#include "indexwindow.h"
#include "ui_indexwindow.h"
#include "creategroupwindow.h"
#include "addfriendswindow.h"
#include "receivefilewindow.h"
#include "loginwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>

IndexWindow::IndexWindow(const QString &username, const QString &img, const QList<User> &friendList, Network *network, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::IndexWindow), network(network), launcher(launcher) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    //初始化用户的昵称，ip信息，头像
    ui->username_label->setText(username);
    ui->ip_label->setText(network->getIp());
    ui->picture->setPixmap(QPixmap::fromImage(QImage(QCoreApplication::applicationDirPath() + "/images/" + img)));

    //初始化好友列表
    this->_friend_list_layout = new QVBoxLayout();
    ui->scrollAreaWidgetContents->setLayout(this->_friend_list_layout);
    for (auto item : friendList) {
        auto fi = new FriendInformation(item);
        if (item.isNewMsg) {
            fi->setNewMessage();
        }
        _friend_list_layout->addWidget(fi);
        connect(fi,&FriendInformation::doubleClickedSignal,this,&IndexWindow::onChatWithFriendSignal);
        connect(fi,&FriendInformation::undefinedButtonClickedSignal,this,&IndexWindow::onDeleteFriendSignal);
    }

    //连接
    connect(ui->add_friend_button,SIGNAL(clicked()),this,SLOT(onAddFriendButtonClicked()));
    connect(ui->create_group_button,SIGNAL(clicked()),this,SLOT(onCreateGroupButtonClicked()));
    connect(ui->changeImageButton, &QPushButton::clicked, this, &IndexWindow::onChangeImgButtonClicked);

    connect(network, &Network::groupMemberListSignal, this, &IndexWindow::onGroupMemberListFeedbackSignal);
    connect(network, &Network::createGroupSuccessSignal, this, &IndexWindow::onCreateGroupSuccessFeedbackSignal);
    connect(network, &Network::createGroupFailSignal, this, &IndexWindow::onCreateGroupFailFeedbackSignal);
    connect(network, &Network::addFriendSuccessSignal, this, &IndexWindow::onAddFriendSuccessFeedbackSignal);
    connect(network, &Network::addFriendFailSignal, this, &IndexWindow::onAddFriendFailFeedbackSignal);
    connect(network, &Network::beAddedSignal, this, &IndexWindow::onBeAddedFeedbackSignal);
    connect(network, &Network::beDeletedSignal, this, &IndexWindow::onBeDeletedFeedbackSignal);
    connect(network, &Network::friendOnlineSignal, this, &IndexWindow::onFriendOnlineFeedbackSignal);
    connect(network, &Network::friendOfflineSignal, this, &IndexWindow::onFriendOfflineFeedbackSignal);
    connect(network, &Network::friendImageChangedSignal, this, &IndexWindow::onFriendImageChangedFeedbackSignal);
    connect(network, &Network::newMsgSignal, this, &IndexWindow::onNewMsgFeedbackSignal);
    connect(network, &Network::requestFileSignal, this, &IndexWindow::onRequestFileFeedbackSignal);
    connect(network, &Network::acceptFileSignal, this, &IndexWindow::onAcceptFileFeedbackSignal);
    connect(network, &Network::rejectFileSignal, this, &IndexWindow::onRejectFileFeedbackSignal);
    connect(network, &Network::historySignal, this, &IndexWindow::onHistoryFeedbackSignal);
    connect(network, &Network::fileListSignal, this, &IndexWindow::onFileListFeedbackSignal);
    connect(network, &Network::sendMsgSuccessSignal, this, &IndexWindow::onSendMsgSuccessSignal);
    connect(network, &Network::disconnectedSignal, this, &IndexWindow::onDisconnectedSignal);
}

/*
 * 函数名称：~IndexWindow(析构函数)
 * 功能描述：删除好友列表中所有UI控件，并析构ui对象
 * 初稿完成时间：2023-8-24
 * 作者：林方裕
 */
IndexWindow::~IndexWindow() {
    delete ui;
}

// 以下是重构的新函数
// 作者：朱炫曦

void IndexWindow::deleteFriendFromUI(const QString &name) {
    FriendInformation *deleted = nullptr;
    for(auto fi:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()){
        if (name == fi->getName()) {
            deleted = fi;
            break;
        }
    }
    _friend_list_layout->removeWidget(deleted);
    disconnect(deleted);
    delete deleted;
}

void IndexWindow::insertFriendToUI(const User &user) {
    QList<FriendInformation *> list;
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        list.append(f);
        _friend_list_layout->removeWidget(f);
    }
    auto fi = new FriendInformation(user);
    _friend_list_layout->addWidget(fi);
    connect(fi,&FriendInformation::doubleClickedSignal,this,&IndexWindow::onChatWithFriendSignal);
    connect(fi,&FriendInformation::undefinedButtonClickedSignal,this,&IndexWindow::onDeleteFriendSignal);
    for (auto f : list) {
        _friend_list_layout->addWidget(f);
    }
}

void IndexWindow::putFriendToFront(const QString &name) {
    QList<FriendInformation *> list;
    FriendInformation *fi = nullptr;
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (f->getName() == name) {
            fi = f;
        }
        list.append(f);
        _friend_list_layout->removeWidget(f);
    }
    list.removeOne(fi);
    list.push_front(fi);
    for (auto f : list) {
        _friend_list_layout->addWidget(f);
    }
}

QString IndexWindow::fileSizeFormatter(qint64 size) {
    double formatedSize = (double) size;
    QString unit;
    if (formatedSize > 1e12) {
        formatedSize /= 1099511627776.0;
        unit = "TB";
    } else if (formatedSize > 1e9) {
        formatedSize /= 1073741824.0;
        unit = "GB";
    } else if (formatedSize > 1e6) {
        formatedSize /= 1048576.0;
        unit = "MB";
    } else if (formatedSize > 1e3) {
        formatedSize /= 1024.0;
        unit = "KB";
    } else {
        unit = "B";
    }
    return QString::number(formatedSize, 'g', 3) + unit;
}

// button slots
void IndexWindow::onAddFriendButtonClicked() {
    launcher->gachaLaunch();
    _add_friends_window = new AddFriendsWindow(ui->username_label->text(), launcher);
    connect(_add_friends_window,&AddFriendsWindow::addFriendRequestSignal, this, &IndexWindow::onAddFriendRequestSignal);
    connect(_add_friends_window,&AddFriendsWindow::windowClosed,this, &IndexWindow::onAddFriendWindowClosed);
    _add_friends_window->show();
    ui->add_friend_button->setDisabled(true);
}

void IndexWindow::onCreateGroupButtonClicked() {
    launcher->gachaLaunch();
    QList<QString> friendList;
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (f->getName()[0] != '_') {
            friendList.append(f->getName());
        }
    }
    _create_group_window = new CreateGroupWindow(friendList, ui->username_label->text(), launcher);  // 这个再想办法
    connect(_create_group_window,&CreateGroupWindow::createGroupRequestSignal,this,&IndexWindow::onCreateGroupRequestSignal);
    connect(_create_group_window,&CreateGroupWindow::windowClosed,this, &IndexWindow::onCreateGroupWindowClosed);
    _create_group_window->show();
    ui->create_group_button->setDisabled(true);
}

void IndexWindow::onChangeImgButtonClicked() {
    launcher->gachaLaunch();
    auto path = QFileDialog::getOpenFileName(this, "打开png图片", "../", "Images (*.png)");
    if (path != "") {
        QImage originalImage(path);
        // 确定裁剪区域以获取正方形部分
        int size = qMin(originalImage.width(), originalImage.height());
        QRect squareRect((originalImage.width() - size) / 2, (originalImage.height() - size) / 2, size, size);
        QImage squareImage = originalImage.copy(squareRect);
        // 将裁剪后的图像调整为128x128像素
        QSize newSize(128, 128);
        squareImage = squareImage.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->picture->setPixmap(QPixmap::fromImage(squareImage));
        QByteArray array;
        QBuffer buffer(&array);
        buffer.open(QIODevice::WriteOnly);
        squareImage.save(&buffer, "PNG");
        QString imgName = QCryptographicHash::hash(array, QCryptographicHash::Md5).toHex() + ".png";
        QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
        if (!file.exists()) {
            squareImage.save(QCoreApplication::applicationDirPath() + "/images/" + imgName, "PNG");
        }
        network->requestChangeImg(imgName);
    }
}

void IndexWindow::onChatWithFriendSignal(FriendInformation* fi) {
    launcher->gachaLaunch();
    QString name = fi->getName();
    if (!_chat_windows.contains(name)) {
        fi->refreshColor();
        ChatWindow* chat_window = new ChatWindow(ui->username_label->text(), name, ui->ip_label->text(), launcher);
        _chat_windows[name] = chat_window;
        connect(chat_window,&ChatWindow::sendMessageRequestSignal,this,&IndexWindow::onSendMessageRequestSignal);
        connect(chat_window,&ChatWindow::transferFileRequestSignal,this,&IndexWindow::onTransferFileRequestSignal);
        connect(chat_window,&ChatWindow::chatHistoryRequestSignal,this,&IndexWindow::onChatHistoryRequestSignal);
        connect(chat_window,&ChatWindow::groupMemberRequestSignal,this,&IndexWindow::onGroupMemberRequestSignal);
        connect(chat_window,&ChatWindow::groupFileQuerySignal,this,&IndexWindow::onGroupFileQuerySignal);
        connect(chat_window,&ChatWindow::groupFileDeleteSignal,this,&IndexWindow::onGroupFileDeleteSignal);
        connect(chat_window,&ChatWindow::groupFileDownloadSignal,this,&IndexWindow::onGroupFileDownloadSignal);
        connect(chat_window,&ChatWindow::groupFileUploadSignal,this,&IndexWindow::onGroupFileUploadSignal);
        connect(chat_window,&ChatWindow::windowClosed, this, &IndexWindow::onChatWindowClosed);
        network->requestLatestHistory(name);
        chat_window->show();
    }
}

void IndexWindow::onDeleteFriendSignal(FriendInformation* fi) {
    launcher->gachaLaunch();
    if (QMessageBox::question(this, tr("删除好友"), tr("确定要删除该好友吗？"),QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
        QString name = fi->getName();
        if (_chat_windows.contains(name)) {
            _chat_windows[name]->close();
        }
        network->requestDeleteFriend(name);
        deleteFriendFromUI(name);
    }
}

// network feedback
void IndexWindow::onGroupMemberListFeedbackSignal(QString groupName, QList<QString> list) {
    if (_chat_windows.contains(groupName)) {
        _chat_windows[groupName]->onGroupMemberSignal(list);
    }
}

void IndexWindow::onCreateGroupSuccessFeedbackSignal() {
    if (_create_group_window != nullptr) {
        _create_group_window->onCreateGroupSuccessSignal();
    }
}

void IndexWindow::onCreateGroupFailFeedbackSignal() {
    if (_create_group_window != nullptr) {
        _create_group_window->onCreateGroupFailSignal();
    }
}

void IndexWindow::onAddFriendSuccessFeedbackSignal(QString name, QString ip, QString imgName) {
    if (_add_friends_window != nullptr) {
        _add_friends_window->onAddFriendSuccess();
    }
    insertFriendToUI({name, imgName, ip, false});
}

void IndexWindow::onAddFriendFailFeedbackSignal(QString name) {
    if (_add_friends_window != nullptr) {
        _add_friends_window->onAddFriendFail();
    }
}

void IndexWindow::onBeAddedFeedbackSignal(QString name, QString ip, QString imgName) {
    if (name[0] != '_') {
        QMessageBox::information(this,"喜报","你已被"+name+"添加为好友！");
    }
    insertFriendToUI({name, imgName, ip, false});
}

void IndexWindow::onBeDeletedFeedbackSignal(QString name) {
    QMessageBox::information(this,"悲报","你已被好友"+name+"删除,玩原神玩的！");
    if(_chat_windows.contains(name)){
        _chat_windows[name]->close();
    }
    deleteFriendFromUI(name);
}

void IndexWindow::onFriendOnlineFeedbackSignal(QString name, QString ip) {
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (f->getName() == name) {
            f->setIp(ip);
            f->refreshColor();
            break;
        }
    }
}

void IndexWindow::onFriendOfflineFeedbackSignal(QString name) {
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (f->getName() == name) {
            f->setIp("");
            f->refreshColor();
            break;
        }
    }
}

void IndexWindow::onFriendImageChangedFeedbackSignal(QString name, QString imgName) {
    for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (f->getName() == name) {
            f->setIcon(imgName);
            break;
        }
    }
}

void IndexWindow::onNewMsgFeedbackSignal(QString innerName, QString sender, QString msg, QString type) {
    putFriendToFront(innerName);
    if (_chat_windows.contains(innerName)) {
        _chat_windows[innerName]->onNewMessageSignal(sender, msg, type);
    } else {
        for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
            if (f->getName() == innerName) {
                f->setNewMessage();
                break;
            }
        }
    }
}

void IndexWindow::onRequestFileFeedbackSignal(QString sender, QString fileName, qint64 size) {
    if (QMessageBox::question(this,"传输文件请求",sender + "请求向您发送文件" + fileName + " (" + IndexWindow::fileSizeFormatter(size) + ")",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes){
        QString path = QFileDialog::getSaveFileName(this,"保存文件", "../" + fileName);
        if (path == "") {
            network->requestRejectFile(sender);
        } else {
            QString ip;
            for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
                if (f->getName() == sender) {
                    ip = f->getIp();
                    break;
                }
            }
            ReceiveFileWindow *w = new ReceiveFileWindow(path, sender, ip);
            connect(w,&ReceiveFileWindow::accept,this, &IndexWindow::onGetPort);
            w->setWindowTitle("接收" + sender + "发来的文件");
            w->show();
        }
    } else {
        network->requestRejectFile(sender);
    }
}

void IndexWindow::onAcceptFileFeedbackSignal(QString receiver, quint16 port) {
    if (_chat_windows.contains(receiver)) {
        QString ip;
        for (auto f : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
            if (f->getName() == receiver) {
                ip = f->getIp();
                break;
            }
        }
        _chat_windows[receiver]->onAcceptTransferFileSignal(ip, port);
    }
}

void IndexWindow::onRejectFileFeedbackSignal(QString receiver) {
    if (_chat_windows.contains(receiver)) {
        _chat_windows[receiver]->onRejectTransferFileSignal();
    }
}

void IndexWindow::onHistoryFeedbackSignal(QString name, QList<ChatRecord> list) {
    if (_chat_windows.contains(name)) {
        _chat_windows[name]->onReceiveHistorySignal(list);
    }
}

void IndexWindow::onFileListFeedbackSignal(QString groupName, QList<GroupFile> list) {
    if (_chat_windows.contains(groupName)) {
        _chat_windows[groupName]->onGroupFileSignal(list);
    }
}

void IndexWindow::onDisconnectedSignal() {
    QMessageBox::information(this,"错误","与服务器断开连接，返回登录窗口");
    close();
}

// add friend window
void IndexWindow::onAddFriendRequestSignal(QString name) {
    for (auto fi : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (name == fi->getName() && _add_friends_window != nullptr) {
            _add_friends_window->onAddFriendAlready();
            return;
        }
    }
    network->requestAddFriend(name);
}

void IndexWindow::onAddFriendWindowClosed() {
    ui->add_friend_button->setDisabled(false);
    _add_friends_window = nullptr;
}

void IndexWindow::onSendMsgSuccessSignal(QString name, QString msg, QString type) {
    if (_chat_windows.contains(name)) {
        _chat_windows[name]->onSendMessageSuccessSignal(msg, type);
    }
}

// create group window
void IndexWindow::onCreateGroupRequestSignal(QString groupName, QString imgName, QList<QString> list) {
    for (auto fi : ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        if (groupName == fi->getName() && _create_group_window != nullptr) {
            _create_group_window->onCreateGroupFailSignal();
            return;
        }
    }
    network->requestCreateGroup(groupName, imgName, list);
}

void IndexWindow::onCreateGroupWindowClosed() {
    ui->create_group_button->setDisabled(false);
    _create_group_window = nullptr;
}

// chat window
void IndexWindow::onChatWindowClosed(QString name) {
    _chat_windows.remove(name);
}

void IndexWindow::onSendMessageRequestSignal(QString name, QString msg, QString type) {
    putFriendToFront(name);
    network->requestSendMsg(name, msg, type);
}

void IndexWindow::onTransferFileRequestSignal(QString receiver, QString fileName, qint64 size) {
    network->requestRequestFile(receiver, fileName, size);
}

void IndexWindow::onChatHistoryRequestSignal(QString name, QDate start, QDate end) {
    network->requestHistory(name, start, end);
}

void IndexWindow::onGroupMemberRequestSignal(QString groupName) {
    network->requestGroupMemberList(groupName);
}

void IndexWindow::onGroupFileQuerySignal(QString groupName) {
    network->requestGroupFileList(groupName);
}

void IndexWindow::onGroupFileDeleteSignal(QString groupName, QString fileName) {
    network->requestDeleteFile(groupName, fileName);
}

void IndexWindow::onGroupFileDownloadSignal(QString groupName, QString fileName, quint16 port) {
    network->requestDownloadFile(groupName, fileName, port);
}

void IndexWindow::onGroupFileUploadSignal(QString groupName, QString fileName, qint64 size, quint16 port) {
    network->requestUploadFile(groupName, fileName, size, port);
}

// sendFile
void IndexWindow::onGetPort(QString name, quint16 port) {
    network->requestAcceptFile(name, port);
}

void IndexWindow::closeEvent(QCloseEvent *) {
    disconnect(network, nullptr, this, nullptr);
    for (ChatWindow* w:_chat_windows) {
        w->close();
    }
    for (FriendInformation* f:ui->scrollAreaWidgetContents->findChildren<FriendInformation*>()) {
        disconnect(f);
        delete f;
    }
    if (_add_friends_window != nullptr) {
        _add_friends_window->close();
    }
    if (_create_group_window != nullptr) {
        _create_group_window->close();
    }
    network->disconnectFromServer();
    auto lw = new LoginWindow(network, launcher, ui->username_label->text());
    lw->show();
}
