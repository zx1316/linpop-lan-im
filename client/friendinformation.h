/*
 * 文件名称：friendinformation.h
 * 类名称：FriendInformation(好友信息)
 * 描述：用于显示好友的昵称、IP地址和在线状态，加入未定义按钮
 * 成员变量：
 * 1.   ui:ChatHistoryWindow*   窗口界面
 * 2.   _undefined_button       连接未定义的控件，使外界可以定义其功能
 * 初版完成时间：2023.08.23
 * 做成时间：----。--。--
 * 作者：林方裕
 */
#ifndef FRIENDINFORMATION_H
#define FRIENDINFORMATION_H

#include <QWidget>
#include <QToolButton>
#include "network.h"
#include "ui_friendinformation.h"

namespace Ui {
class FriendInformation;
}

class FriendInformation : public QWidget {
    Q_OBJECT

private:
    Ui::FriendInformation *ui;
    QString name;   // 这个东西现在是绝对的主键了！！！
    bool newMsg = false;

public:
    explicit FriendInformation(const User& u, QWidget *parent = nullptr);
    ~FriendInformation();
    void setNewMessage();
    void refreshColor();
    void setIp(const QString &ip);
    QString getIp() const;
    QString getName() const;
    void setIcon(const QString& imgName);
    bool isNewMsg() const;

private slots:
    void onUndefinedButtonClicked();
    void onDoubleClickedSignal();

signals:
    void undefinedButtonClickedSignal(FriendInformation* uf);
    void doubleClickedSignal(FriendInformation* uf);
};

#endif // FRIENDINFORMATION_H
