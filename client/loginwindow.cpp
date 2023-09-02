#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "registrationwindow.h"
#include "indexwindow.h"
#include "QInputDialog"
#include "video.h"
#include "configwindow.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    ui->user_line_edit->setPlaceholderText("输入用户名");
    ui->password_line_edit->setPlaceholderText("输入密码");
    //绑定文本编辑框回车键触发跳转槽函数
    connect(ui->user_line_edit,SIGNAL(returnPressed()),this,SLOT(loginNext()));
    connect(ui->password_line_edit,SIGNAL(returnPressed()),this,SLOT(onLoginPushButtonClicked()));
    connect(ui->config_pushbutton, &QPushButton::clicked, this, &LoginWindow::onConfigClicked);
    connect(ui->login_pushbutton,SIGNAL(clicked()),this,SLOT(onLoginPushButtonClicked()));
    connect(ui->signup_pushbutton,SIGNAL(clicked()),this,SLOT(onSignupPushButtonClicked()));
/*
    QString ip = QInputDialog::getText(this,"输入IP","输入IP",QLineEdit::Normal);
    if(!ip.isEmpty()){
        _ip = ip;
        int port = QInputDialog::getInt(this,"输入端口","输入端口",QLineEdit::Normal);
        _port = port;
    }*/
    _client = new RequestToServer();
    connect(_client,&RequestToServer::loginInSignal,this,&LoginWindow::onLoginFeedbackSignal);
}

LoginWindow::~LoginWindow()
{
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
    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::critical(this, "登录失败", "用户名或密码不能为空");    //不能为空错误提示
        return;
    }
//    _client->socketConnect(_ip, _port);
    ui->login_pushbutton->setEnabled(false);
    _client->requestLogin(username,password, _ip, _port);
}

void LoginWindow::onLoginFeedbackSignal(int feedback){
    ui->login_pushbutton->setEnabled(true);
    switch (feedback)
    {
        case 3:
            QMessageBox::critical(this,"网络错误","无法连接到服务器，请检查服务器设置或网络连接");break;
        case 1:
            QMessageBox::critical(this, "登录失败", "用户名或密码错误");
            break;
        case 2:
            QMessageBox::critical(this, "登录失败", "用户已在线");
            break;
        case 0:
            Video *v = new Video(_client, ui->user_line_edit->text());
            v->showFullScreen();
//            v->show();
            this->close();
            break;
    }
}
void LoginWindow::onSignupPushButtonClicked()
{
    ui->user_line_edit->clear();
    ui->password_line_edit->clear();
    this->close();
    RegistrationWindow *r = new RegistrationWindow(this,_ip,_port, _client);
    r->show();
}

void LoginWindow::onConfigClicked() {
    ConfigWindow *w = new ConfigWindow(nullptr, _ip, &_port);
    w->show();
}
