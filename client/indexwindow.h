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
#include "chatwindow.h"

namespace Ui {
class IndexWindow;
}

class IndexWindow : public QWidget {
    Q_OBJECT

public:
    IndexWindow(const QString& username, const QString& img, const QList<User>& friendList, QWidget *parent = nullptr);
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
    void deleteFriendFromUI(const QString& name);
    void insertFriendToUI(const User& user);
    void putFriendToFront(const QString& name);

public slots:
    // button slots
    void onAddFriendButtonClicked();
    void onCreateGroupButtonClicked();
    void onChangeImgButtonClicked();
    void onChatWithFriendSignal(FriendInformation* fi);
    void onDeleteFriendSignal(FriendInformation* fi);

    // network feedback
    void onGroupMemberListFeedbackSignal(const QString& groupName, const QList<QString>& list);
    void onCreateGroupSuccessFeedbackSignal();
    void onCreateGroupFailFeedbackSignal();
    void onAddFriendSuccessFeedbackSignal(const QString& name, const QString& ip, const QString& imgName);
    void onAddFriendFailFeedbackSignal();
    void onBeAddedFeedbackSignal(const QString& name, const QString& ip, const QString& imgName);
    void onBeDeletedFeedbackSignal(const QString& name);
    void onFriendOnlineFeedbackSignal(const QString& name, const QString& ip);
    void onFriendOfflineFeedbackSignal(const QString& name);
    void onFriendImageChangedFeedbackSignal(const QString& name, const QString& imgName);
    void onNewMsgFeedbackSignal(const QString& innerName, const QString& sender, const QString& msg, const QString& type);
    void onRequestFileFeedbackSignal(const QString& sender, const QString& fileName, qint64 size);
    void onAcceptFileFeedbackSignal(const QString& receiver, quint16 port);
    void onRejectFileFeedbackSignal(const QString& receiver);
    void onHistoryFeedbackSignal(const QString& name, const QList<ChatRecord>& list);
    void onFileListFeedbackSignal(const QString& groupName, const QList<GroupFile>& list);
    void onSendMsgSuccessSignal(const QString& name, const QString& msg, const QString& type);
    void onDisconnectedSignal();

    // chat window
    void onAddFriendRequestSignal(const QString&);
    void onAddFriendWindowClosed();

    // create group window
    void onCreateGroupRequestSignal(const QString& groupName, const QString& imgName, const QList<QString>& list);
    void onCreateGroupWindowClosed();

    // chat window
    void onChatWindowClosed(const QString& name);
    void onSendMessageRequestSignal(const QString& name, const QString& msg, const QString& type);
    void onTransferFileRequestSignal(const QString& receiver, const QString& fileName, qint64 size);
    void onChatHistoryRequestSignal(const QString& name, const QDate& start, const QDate& end);
    void onGroupMemberRequestSignal(const QString& groupName);
    void onGroupFileQuerySignal(const QString& groupName);
    void onGroupFileDeleteSignal(const QString& groupName, const QString& fileName);
    void onGroupFileDownloadSignal(const QString& groupName, const QString& fileName, quint16 port);
    void onGroupFileUploadSignal(const QString& groupName, const QString& fileName, qint64 size, quint16 port);

    // sendFile
    void onGetPort(const QString&, quint16);

};

#endif // INDEXWINDOW_H
