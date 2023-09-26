#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "registrationwindow.h"
#include "indexwindow.h"
#include "video.h"
#include "configwindow.h"
#include <QCryptographicHash>
#include <QDir>

LoginWindow::LoginWindow(Network *network, MiHoYoLauncher *launcher, const QString &name, QWidget *parent) : QWidget(parent), ui(new Ui::LoginWindow), network(network), launcher(launcher) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    QDir dir(QCoreApplication::applicationDirPath());
    dir.mkdir("images");
    ui->user_line_edit->setPlaceholderText("输入用户名");
    ui->password_line_edit->setPlaceholderText("输入密码");
    ui->user_line_edit->setText(name);
    //绑定文本编辑框回车键触发跳转槽函数
    connect(ui->user_line_edit,SIGNAL(returnPressed()),this,SLOT(loginNext()));
    connect(ui->password_line_edit,SIGNAL(returnPressed()),this,SLOT(onLoginPushButtonClicked()));
    connect(ui->config_pushbutton, &QPushButton::clicked, this, &LoginWindow::onConfigClicked);
    connect(ui->login_pushbutton,SIGNAL(clicked()),this,SLOT(onLoginPushButtonClicked()));
    connect(ui->signup_pushbutton,SIGNAL(clicked()),this,SLOT(onSignupPushButtonClicked()));
    connect(network, &Network::loginAlreadySignal, this, &LoginWindow::onloginAlready);
    connect(network, &Network::loginUnauthorizedSignal, this, &LoginWindow::onloginUnauthorized);
    connect(network, &Network::loginSuccessSignal, this, &LoginWindow::onloginSuccess);
    connect(network, &Network::connectedSignal, this, &LoginWindow::onNetworkConnected);
    connect(network, &Network::disconnectedSignal, this, &LoginWindow::onNetworkDisconnected);
}

LoginWindow::~LoginWindow() {
    delete ui;
}

/*
 * 名称：login_Next
 * 作用：在登录界面，用户名一栏按回车键跳转密码输入，密码输入一栏按回车键跳转登录尝试
 */
void LoginWindow::loginNext()
{
    QLineEdit *line_edit = (QLineEdit *)sender();
        if (line_edit == ui->user_line_edit)
        {
            ui->password_line_edit->setFocus();//在用户名文本编辑框跳转密码文本编辑框
        }

}

/* 名称：onLoginPushButtonClicked()
 * 作用：点击登录按键后验证用户状态，并给出错误提示或进入主界面。
 */
void LoginWindow::onLoginPushButtonClicked()
{
    QString username = ui->user_line_edit->text();
    QString password = ui->password_line_edit->text();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::critical(this, "登录失败", "用户名或密码不能为空");    //不能为空错误提示
        return;
    }
    ui->login_pushbutton->setEnabled(false);
    network->connectToServer();
}

void LoginWindow::onSignupPushButtonClicked() {
    RegistrationWindow *r = new RegistrationWindow(network, launcher);
    r->show();
    this->close();
}

void LoginWindow::onConfigClicked() {
    ConfigWindow *w = new ConfigWindow(network);
    w->show();
}

void LoginWindow::onloginAlready() {
    QMessageBox::critical(this, "登录失败", "用户已在线");
    ui->login_pushbutton->setEnabled(true);
}

void LoginWindow::onloginUnauthorized() {
    QMessageBox::critical(nullptr, "登录失败", "用户名或密码错误");
    ui->login_pushbutton->setEnabled(true);
}

void LoginWindow::onloginSuccess(QString imgName, QList<User> list) {
    // 开始放视频喽
    IndexWindow *w = new IndexWindow(ui->user_line_edit->text(), imgName, list, network, launcher);
    Video *v = new Video(w);
    v->showFullScreen();
    this->close();
}

void LoginWindow::onNetworkConnected() {
    connectFlag = true;
    auto name = ui->user_line_edit->text();
    auto pwd = ui->password_line_edit->text();
    network->requestLogin(ui->user_line_edit->text(), QCryptographicHash::hash((pwd + name).toUtf8(), QCryptographicHash::Md5).toHex());
}

void LoginWindow::onNetworkDisconnected() {
    if (connectFlag == false) {
        QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查服务器设置或网络连接");
    }
    connectFlag = false;
    ui->login_pushbutton->setEnabled(true);
}
