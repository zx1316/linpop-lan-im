#include "modifynicknamewindow.h"
#include "ui_modifynicknamewindow.h"

ModifyNicknameWindow::ModifyNicknameWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifyNicknameWindow)
{
    ui->setupUi(this);

    QString ori_nickname = "原本昵称";//占位，此处应是用户自己的昵称
    ui->lineEdit_NickName->setText(ori_nickname);
}

ModifyNicknameWindow::~ModifyNicknameWindow()
{
    delete ui;
}

/*
 * 名称：UpdateNickname
 * 作用：修改昵称，并返回成功或失败状态
 * 返回值： 0-失败 1-成功 (?????)
 */
int updateNickname(QString newname)
{
    return 0;
}





void ModifyNicknameWindow::on_pushButton_Modify_clicked()
{
    QString username = ui->lineEdit_NickName->text();
    int search_state = updateNickname(username);
    if(search_state==1)
    {
        QMessageBox::information(this,"修改完毕", "修改成功！");
    }
    else
    {
        QMessageBox::critical(this,"修改完毕", "修改失败");
    }
}
