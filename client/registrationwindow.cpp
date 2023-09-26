#include "registrationwindow.h"
#include "ui_registrationwindow.h"
#include "loginwindow.h"
#include <QFileDialog>
#include <QCryptographicHash>
#include <QBuffer>

RegistrationWindow::RegistrationWindow(Network *network, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::RegistrationWindow), network(network), launcher(launcher) {
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    //绑定回车键触发跳转槽函数
    connect(ui->re_username_line_edit,SIGNAL(returnPressed()),this,SLOT(registrationNext()));
    connect(ui->re_password_line_edit,SIGNAL(returnPressed()),this,SLOT(registrationNext()));
    connect(ui->re_password_confirm_line_edit,SIGNAL(returnPressed()),this,SLOT(onRegistrationPushButtonClicked()));

    connect(ui->registration_pushbutton,SIGNAL(clicked()),this,SLOT(onRegistrationPushButtonClicked()));
    connect(ui->back_to_login_pushbutton, &QPushButton::clicked, this, &RegistrationWindow::close);
    connect(ui->selectImageButton, &QPushButton::clicked, this, &RegistrationWindow::onSelectImgButtonClicked);
    connect(network, &Network::connectedSignal, this, &RegistrationWindow::onNetworkConnected);
    connect(network, &Network::disconnectedSignal, this, &RegistrationWindow::onNetworkDisconnected);
    connect(network, &Network::registerSuccessSignal, this, &RegistrationWindow::onRegisterSuccess);
    connect(network, &Network::registerFailSignal, this, &RegistrationWindow::onRegisterFail);
}

RegistrationWindow::~RegistrationWindow() {
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
    if(username.isEmpty()||password.isEmpty()||password_confirmed.isEmpty()) { //为空报错
        QMessageBox::critical(this,"注册失败","输入框不能为空");
        return;
    }
    if(username.startsWith('_')){
        QMessageBox::critical(this,"注册失败","用户名不能以下划线开头");
        return;
    }
    int spaceCnt = 0;
    for (auto ch : username) {
        if (ch.isSpace()) {
            spaceCnt++;
        }
    }
    if (spaceCnt == username.length()) {
        QMessageBox::critical(this,"注册失败","用户名不能全为空格");
        return;
    }
    if(username.toUtf8().size() > 30){
        QMessageBox::critical(this,"注册失败","用户名过长");
        return;   // 谁他妈忘了return？
    }
    if(password.compare(password_confirmed)){//密码不一致报错
        QMessageBox::critical(this,"注册失败","两次密码输入不一致");
        return;
    }
    if (ui->imageLabel->pixmap() == nullptr) {
        QMessageBox::critical(this,"注册失败","请设置头像");
        return;
    }
    network->connectToServer();
}

/*名称：closeEvent
 * 作用：关闭后确认是否回到登陆界面
 */
void RegistrationWindow::closeEvent(QCloseEvent *event) {
    auto lw = new LoginWindow(network, launcher, "");
    lw->show();
}

void RegistrationWindow::onNetworkConnected() {
    connectFlag = true;
    QString name = ui->re_username_line_edit->text();
    QString pwd = ui->re_password_line_edit->text();
    auto image = ui->imageLabel->pixmap()->toImage();
    QByteArray array;
    QBuffer buffer(&array);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    QString imgName = QCryptographicHash::hash(array, QCryptographicHash::Md5).toHex() + ".png";
    QFile file(QCoreApplication::applicationDirPath() + "/images/" + imgName);
    if (!file.exists()) {
        image.save(QCoreApplication::applicationDirPath() + "/images/" + imgName, "PNG");
    }
    network->requestRegister(name, QCryptographicHash::hash((pwd + name).toUtf8(), QCryptographicHash::Md5).toHex(), imgName);
    ui->registration_pushbutton->setEnabled(false);
}

void RegistrationWindow::onNetworkDisconnected() {
    if (connectFlag == false) {
        QMessageBox::critical(this, "网络错误", "无法连接到服务器，请检查服务器设置或网络连接");
    }
    connectFlag = false;
    ui->registration_pushbutton->setEnabled(true);
}

void RegistrationWindow::onRegisterSuccess() {
    QMessageBox::information(this,"注册成功","转到登录窗口");
    this->close();
}

void RegistrationWindow::onRegisterFail() {
    QMessageBox::critical(this,"注册失败","用户名已被注册");
    ui->registration_pushbutton->setEnabled(true);
}

void RegistrationWindow::onSelectImgButtonClicked() {
    auto path = QFileDialog::getOpenFileName(this, "打开png图片", "../", "Images (*.png)");
    if (path != "") {
        QImage originalImage(path);
        // 确定裁剪区域以获取正方形部分
        int size = qMin(originalImage.width(), originalImage.height());
        QRect squareRect((originalImage.width() - size) / 2, (originalImage.height() - size) / 2, size, size);
        QImage squareImage = originalImage.copy(squareRect);
        // 将裁剪后的图像调整为128x128像素
        QSize newSize(128, 128);
        squareImage = squareImage.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->imageLabel->setPixmap(QPixmap::fromImage(squareImage));
    }
}
