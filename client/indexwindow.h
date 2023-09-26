/*
 * 文件名称：indexwindow.h
 * 类名称：IndexWindow（主界面窗口）
 * 描述：用于显示个人昵称和IP地址、好友列表，连接添加好友、创建群聊、进入聊天界面和删除好友多个功能，并作为这些功能与网络的中枢接口
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：林方裕
 */
#ifndef INDEXWINDOW_H
#define INDEXWINDOW_H

#include <QWidget>
#include "friendinformation.h"
#include "addfriendswindow.h"
#include "creategroupwindow.h"
#include "mihoyolauncher.h"
#include "chatwindow.h"
#include "network.h"

namespace Ui {
class IndexWindow;
}

class IndexWindow : public QWidget {
    Q_OBJECT

public:
    explicit IndexWindow(const QString &username, const QString &img, const QList<User> &friendList, Network *network, MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~IndexWindow();
    static QString fileSizeFormatter(qint64 size);

protected:
    void closeEvent(QCloseEvent *) override;

private:
    Ui::IndexWindow *ui;
    QLayout *_friend_list_layout;//存放好友列表UI界面
    QHash<QString, ChatWindow *> _chat_windows;
    AddFriendsWindow *_add_friends_window = nullptr;
    CreateGroupWindow *_create_group_window = nullptr;
    Network *network;
    MiHoYoLauncher *launcher;
    void deleteFriendFromUI(const QString &name);
    void insertFriendToUI(const User &user);
    void putFriendToFront(const QString &name);

public slots:
    // button slots
    void onAddFriendButtonClicked();
    void onCreateGroupButtonClicked();
    void onChangeImgButtonClicked();
    void onChatWithFriendSignal(FriendInformation* fi);
    void onDeleteFriendSignal(FriendInformation* fi);

    // network feedback
    void onGroupMemberListFeedbackSignal(QString groupName, QList<QString> list);
    void onCreateGroupSuccessFeedbackSignal();
    void onCreateGroupFailFeedbackSignal();
    void onAddFriendSuccessFeedbackSignal(QString name, QString ip, QString imgName);
    void onAddFriendFailFeedbackSignal(QString name);
    void onBeAddedFeedbackSignal(QString name, QString ip, QString imgName);
    void onBeDeletedFeedbackSignal(QString name);
    void onFriendOnlineFeedbackSignal(QString name, QString ip);
    void onFriendOfflineFeedbackSignal(QString name);
    void onFriendImageChangedFeedbackSignal(QString name, QString imgName);
    void onNewMsgFeedbackSignal(QString innerName, QString sender, QString msg, QString type);
    void onRequestFileFeedbackSignal(QString sender, QString fileName, qint64 size);
    void onAcceptFileFeedbackSignal(QString receiver, quint16 port);
    void onRejectFileFeedbackSignal(QString receiver);
    void onHistoryFeedbackSignal(QString name, QList<ChatRecord> list);
    void onFileListFeedbackSignal(QString groupName, QList<GroupFile> list);
    void onSendMsgSuccessSignal(QString name, QString msg, QString type);
    void onDisconnectedSignal();

    // chat window
    void onAddFriendRequestSignal(QString);
    void onAddFriendWindowClosed();

    // create group window
    void onCreateGroupRequestSignal(QString groupName, QString imgName, QList<QString> list);
    void onCreateGroupWindowClosed();

    // chat window
    void onChatWindowClosed(QString name);
    void onSendMessageRequestSignal(QString name, QString msg, QString type);
    void onTransferFileRequestSignal(QString receiver, QString fileName, qint64 size);
    void onChatHistoryRequestSignal(QString name, QDate start, QDate end);
    void onGroupMemberRequestSignal(QString groupName);
    void onGroupFileQuerySignal(QString groupName);
    void onGroupFileDeleteSignal(QString groupName, QString fileName);
    void onGroupFileDownloadSignal(QString groupName, QString fileName, quint16 port);
    void onGroupFileUploadSignal(QString groupName, QString fileName, qint64 size, quint16 port);

    // sendFile
    void onGetPort(QString, quint16);

};

#endif // INDEXWINDOW_H
