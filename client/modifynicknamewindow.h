/*
 * 修改昵称
 */
#ifndef MODIFYNICKNAMEWINDOW_H
#define MODIFYNICKNAMEWINDOW_H

#include <QWidget>
#include <QMessageBox>//

namespace Ui {
class ModifyNicknameWindow;
}

class ModifyNicknameWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ModifyNicknameWindow(QWidget *parent = nullptr);
    ~ModifyNicknameWindow();

private slots:
    void on_pushButton_Modify_clicked();

private:
    void SetNickName();
    Ui::ModifyNicknameWindow *ui;
};

#endif // MODIFYNICKNAMEWINDOW_H
