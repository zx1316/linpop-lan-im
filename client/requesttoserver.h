#ifndef REQUESTTOSERVER_H
#define REQUESTTOSERVER_H

#include <QColor>
#include <QObject>
#include <QTcpSocket>
#include "friendlybufferhelper.h"
#include "friend.h"
#include "chatrecord.h"
#include "user.h"
class RequestToServer : public QObject
{
    Q_OBJECT
public:
    explicit RequestToServer(QObject *parent = nullptr);
    QTcpSocket m_tcp;
    //void socketConnect(QString, quint16);
    void requestAgreeSendFileToMe(User u, quint16 port);  //0 我方接受文件传输
    void requestRefuseSendFileToMe(User u); //1 我方拒绝文件传输
    void requestCreateGroup(QString group_name); //2 创建群聊
    void requestAddFriend(User);      //3
    void requestDeleteFriend(User u);   //4
    void requestChatRecord(User u);                        //5
    void requestChatRecord(User, QDate start, QDate end);          //6
    void requestRegister(QString, QString, QString, quint16);                 //7
    void requestLogin(QString, QString, QString, quint16);                    //8
    void requestSendFile(QString receiver, QString file_name, qint64 file_size);
    void deleteFriendSuccessfully(QByteArray arr);
    void requestChat(User, QString, int, int, QColor);      //11
    void requestFriendList();

    void receiveAgreeSendFileFromOther(QByteArray arr);  //对方接受文件传输请求
    void receiveRefuseSendFileFromOther(QByteArray arr);   //文件发送请求被拒绝
    void getChatRecord(QByteArray arr);
    void getMessage(QByteArray arr);
    void receiveFriendStateUpdate(QByteArray in);
    bool receiveRegister(qint8);
    int receiveLogin(qint8,QByteArray arr);
    void messageSendSuccessfully(QByteArray);
    void addFriendSuccessfully(QByteArray arr);     //7
    void informationOfReceiveFile(QByteArray arr);  //9
    void createGroupSuccessfully(QByteArray arr);
    void requestSendChatFail(QByteArray arr);  //18
    void friendOnline(QByteArray arr);        //好友上线
    void friendOffline(QByteArray);             //20
    void addedAsFriend(QByteArray arr);       //21
    void deletedByFriend(QByteArray arr);    //22
    QString getLocalAddress();



signals:
    void registrationSignal(int); // 1成功 0重名 -1连不上
    void loginInSignal(int);
    void friendListFeedbackSignal(QVector<Friend>);
    void addFriendFeedbackSignal(int,Friend);
    void deleteFriendFeedbackSignal(bool,QString);
    void sendMessageFeedbackSignal(QString,bool);
    void newMessageSignal(QString,QString,QString,QColor,bool,bool,bool,int);
    void chatRecordFeedbackSignal(QString,QVector<ChatRecord>);
    void friendUpSignal(Friend);
    void friendDownSignal(QString);
    void createGroupFeedbackSignal(bool,QString);
    void beAddedSignal(Friend);
    void beDeletedSignal(QString);
    void sendFileFeedbackSignal(bool,QString,qint64);
    void beRequestedSentFileSignal(QString,QString,qint64);
    void connectedSignal();
    void disconnectedSignal();

private:
//    QString ip;
//    unsigned short port;
    QVector<Friend> friendList;
    QHash<QString, QVector<ChatRecord>> qHash;
    int remaining = 0;
    char recvArr[1408];
    int loginOrRegister = 0;
    void handleResponse(qint8 x,QByteArray& arr);

};

#endif // REQUESTTOSERVER_H
