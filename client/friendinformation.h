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
#include <friend.h>
#include <QToolButton>
#include <chatwindow.h>

namespace Ui {
class FriendInformation;
}

class FriendInformation : public QWidget
{
    Q_OBJECT

public:
    explicit FriendInformation(Friend& f,QWidget *parent = nullptr);
    ~FriendInformation();
    QToolButton* undefined_button;
    QString username();
    void setBackgroundColor(bool);
    static QString mapStringToPicture(QString &inputString);
    bool _real;
private:
    Ui::FriendInformation *ui;
    bool _state;
public slots:
    void onUndefinedButtonClicked();
    void onDoubleClickedSignal();
signals:
    void undefinedButtonClickedSignal(FriendInformation* uf);
    void doubleClickedSignal(FriendInformation* uf);
};

#endif // FRIENDINFORMATION_H
