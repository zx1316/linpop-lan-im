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
#include <friend.h>
#include "friendinformation.h"
#include "addfriendswindow.h"
#include "creategroupwindow.h"
#include "requesttoserver.h"
#include "mihoyolauncher.h"

namespace Ui {
class IndexWindow;
}

class IndexWindow : public QWidget
{
    Q_OBJECT

public:
    explicit IndexWindow(QString nickname,QString ip,RequestToServer* client, MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~IndexWindow();
private:
    Ui::IndexWindow *ui;
    QLayout* _friend_list_layout;//存放好友列表UI界面
    QVector<Friend*> _friend_list;//存放好友信息
    QHash<QString,ChatWindow*> _chat_windows;
    AddFriendsWindow* _add_friends_window;
    CreateGroupWindow * _create_group_window;
    RequestToServer* _client;
    MiHoYoLauncher *launcher;
    QVector<QString> _member_list;
    void updateFriendList(int,Friend);
    void updateUiList();
    void deleteFriendFromUI(Friend);

public slots:
    //initSlots
    void onFriendListFeedbackSignal(QVector<Friend> friends);

    //ButtonSlots
    void onAddFriendButtonClicked();
    void onCreateGroupButtonClicked();
    void onChatWithFriendSignal(FriendInformation* fi);

    //AddFriend
    void onAddFriendRequestSignal(bool,QString);
    void onAddFriendFeedbackSignal(int,Friend);

    //CreateGroup
    void onCreateGroupRequestSignal(QString,QVector<QString>);
    void onCreateGroupFeedbackSignal(bool,QString);


    //ChatWithFriend
    void onNewMessageSignal(QString,QString,QString,QColor,bool,bool,bool,int);
    void onSendMessageRequestSignal(QString,QString,QColor,bool,bool,bool,int);
    void onSendMessageFeedbackSignal(QString,bool);

    //query for chat record
    void onChatRecentRecordRequestSignal(QString);
    void onChatRecentRecordFeedbackSignal(QString username,QVector<ChatRecord>);
    void onChatHistoryRequestSignal(QString,QDate,QDate);;
//    void onChatHistoryFeedbackSignal(QString,QVector<ChatRecord>);

    //sentFile
    void onTransferFileRequestSignal(QString,QString,qint64);
    void onTransferFileFeedbackSignal(bool,QString,int);
    void onAcceptFileSignal(QString,QString,qint64);
    void onGetPort(QString, quint16);

    //deleteFriend
    void onDeleteFriendSignal(FriendInformation* fi);
    void onDeleteFriendFeedbackSignal(bool,QString);
    void onBeDeletedFriendSignal(QString);

    void onFriendUpSignal(Friend);
    void onFriendDownSignal(QString);
    void onBeAddedSignal(Friend f);

    void onTestButtonClicked();

    void onCloseWindow(QWidget*);

signals:
    //AddFriend
    void addFriendRequestSignal(QString);
    void addFriendFeedbackSignal(bool);

    //CreateGroup
    void createGroupRequestSignal(QVector<QString>);
    void createGroupFeedbackSignal(bool);

    //ChatWithFriend
    void newMessageSignal(QString,QString);
    void sendMessageRequestSignal(QString,QString,QColor,bool,bool,bool,int);

    //query for chat record
    void chatHistoryRequestSignal(QString,QDate,QDate);
    void chatHistoryFeedbackSignal(QVector<ChatRecord>);

    //sentFile
    void transferFileRequestSignal(QString,QString);

    //deleteFriend
    void deleteFriendRequestSignal(QString);
    void deleteFriendFeedbackSignal(bool);

};

#endif // INDEXWINDOW_H
