/*
 * 文件名称：loginwindow.h
 * 类名称：LoginWindow（登录窗口）
 * 描述：用于用户输入用户名和密码，进行登录
 * 成员变量：
 * 1.   ui:LoginWindow*         窗口界面
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：刘毅葳
 *
 * 第一次修改
 * 修改时间：2023.8.25
 * 修改人：林方裕
 * 修改内容：拆分点击登录按钮触发的函数，使其分为点击登录按钮发送信号和接收到返回信号时登录两部分
 *
 * 第二次修改
 * 修改时间：2023.8.26
 * 修改人：林方裕
 * 修改内容：哈希加盐算法的接口（未实现）
 */
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QCloseEvent>
#include <QMessageBox>//
#include <QMetaObject>
#include <requesttoserver.h>
QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginPushButtonClicked();

    void onLoginFeedbackSignal(int);

    void onSignupPushButtonClicked();
    void onConfigClicked();

    void loginNext();
private:
    Ui::LoginWindow *ui;
    RequestToServer* _client;
    QString _ip;
    int _port = 8848;
signals:
    void loginRequest(QString,QString);
};
#endif // LOGINWINDOW_H
