/**
 * 文件名称：registrationwindow.h
 * 类名称：RegistrationWindow（注册窗口）
 * 描述：用于注册账号的窗口
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：刘毅葳
 *
 * 第一次修改
 * 修改时间：2023.8.25
 * 修改人：林方裕
 * 修改内容：添加成员变量_login_window，并修改对应构造函数，使其直接返回原来的登录窗口，而不新建对象。
 *
 * 第二次修改
 * 修改时间：2023.9.1
 * 修改人：朱炫曦
 * 修改内容：求求了不要再new一个网络对象出来了，直接传登录窗口的不好吗
 */
#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QWidget>
#include <loginwindow.h>
#include <network.h>

namespace Ui {
class RegistrationWindow;
}

class RegistrationWindow : public QWidget {
    Q_OBJECT

public:
    RegistrationWindow(Network *network, MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~RegistrationWindow();
    void closeEvent(QCloseEvent *);

private slots:
    void onRegistrationPushButtonClicked();
    void registrationNext();
    void onSelectImgButtonClicked();

    void onNetworkConnected();
    void onNetworkDisconnected();
    void onRegisterSuccess();
    void onRegisterFail();

private:
    Ui::RegistrationWindow *ui;
    Network *network;
    MiHoYoLauncher *launcher;
    bool connectFlag = false;
};

#endif // REGISTRATIONWINDOW_H
