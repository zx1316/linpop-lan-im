#include "registrationwindow.h"
#include "ui_registrationwindow.h"
#include "loginwindow.h"
RegistrationWindow::RegistrationWindow(LoginWindow* login_window,QString ip,int port, RequestToServer *client, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationWindow),
    _login_window(login_window),_ip(ip),_port(port),_client(client)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    _registration_state=true;
    //绑定回车键触发跳转槽函数
    connect(ui->re_username_line_edit,SIGNAL(returnPressed()),this,SLOT(registrationNext()));
    connect(ui->re_password_line_edit,SIGNAL(returnPressed()),this,SLOT(registrationNext()));
    connect(ui->re_password_confirm_line_edit,SIGNAL(returnPressed()),this,SLOT(onRegistrationPushButtonClicked()));

    connect(ui->registration_pushbutton,SIGNAL(clicked()),this,SLOT(onRegistrationPushButtonClicked()));
    connect(ui->back_to_login_pushbutton,SIGNAL(clicked()),this,SLOT(onBackToLoginPushButtonClicked()));
    connect(_client,&RequestToServer::registrationSignal,this,&RegistrationWindow::onRegistrationFeedbackSignal);

}

RegistrationWindow::~RegistrationWindow()
{
    delete ui;
}

/*
 * 名称：registrationNext
 * 作用：在注册界面实现回车跳转
 */
void RegistrationWindow::registrationNext()
{
    QLineEdit *line_edit = (QLineEdit *)sender();
    if(line_edit == ui->re_username_line_edit)
    {
        ui->re_password_line_edit->setFocus();//在用户名文本编辑框跳转密码文本编辑框
    }
    else if(line_edit == ui->re_password_line_edit)
    {
        ui->re_password_confirm_line_edit->setFocus();//在密码文本编辑框跳转密码确认文本编辑框
    }

}

/*名称：onLoginPushButtonClicked()
 *作用：点击注册按键后验证用户状态，并给出错误提示或回到登陆界面。
 */
void RegistrationWindow::onRegistrationPushButtonClicked()
{
    QString username = ui->re_username_line_edit->text();
    QString password = ui->re_password_line_edit->text();
    QString password_confirmed = ui->re_password_confirm_line_edit->text();
    if(username.isEmpty()||password.isEmpty()||password_confirmed.isEmpty())//为空报错
    {
        QMessageBox::information(this,"注册失败","输入框不能为空");
        return;
    }
    if(username.startsWith('_')){
        QMessageBox::information(this,"注册失败","用户名不能以下划线开头");
        return;
    }
    if(username.toUtf8().size()>30){
        QMessageBox::information(this,"注册失败","用户名过长");
        return;   // 谁他妈忘了return？
    }
    if(password.compare(password_confirmed)){//密码不一致报错
        QMessageBox::information(this,"注册失败","两次密码输入不一致");
        return;
    }
    qDebug("RegistrationWindow sent registration request");
//    _client->socketConnect(_ip, _port);
    _client->requestRegister(username,password, _ip, _port);
}

void RegistrationWindow::onRegistrationFeedbackSignal(int feedback){
    if(feedback == 0)//用户已存在报错
    {
        qDebug("Registration Error_1");
        QMessageBox::critical(this,"注册失败","用户已存在");
    }else if (feedback == 1)//注册成功
    {
        qDebug("Registartion Success");//写入登录成功
        QMessageBox::information(this,"注册成功","转到登陆页面");
        _registration_state=false;
        _login_window->show();
        this->close();
    } else if (feedback == -1) {
        qDebug("Registration Error_2");
        QMessageBox::critical(this,"网络错误","无法连接至服务器，请检查服务器设置或网络连接");
    }
}

/*名称：closeEvent
 * 作用：关闭后确认是否回到登陆界面
 */
void RegistrationWindow::closeEvent(QCloseEvent *event)
{
    if(!_registration_state){
        this->close();
        return;
    }
    if(QMessageBox::question(this,
                             tr("退出"),
                             tr("回到登陆界面？"),
                             QMessageBox::Yes, QMessageBox::No )
                   == QMessageBox::Yes)
    {
        _login_window->show();
        this->close();
    }
    else
    {
        event->ignore();
    }

}

/*
 * 名称：onBackToLoginClicked()
 * 作用：点击回到登录界面后，关闭注册界面并回到登陆界面
 */
void RegistrationWindow::onBackToLoginPushButtonClicked()
{
    this->close();
}
