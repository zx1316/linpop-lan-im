#ifndef CREATEGROUPWINDOW_H
#define CREATEGROUPWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <friendinformation.h>

namespace Ui {
class CreateGroupWindow;
}

class CreateGroupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CreateGroupWindow(QVector<Friend*>,QWidget *parent = nullptr);
    ~CreateGroupWindow();

private:
    Ui::CreateGroupWindow *ui;
    QLayout *not_selected_layout,*selected_layout;
    void init(QVector<Friend*>);//初始化未选择好友框

protected:
    void closeEvent(QCloseEvent *event) override;
public slots:
    void onCreateGroupButtonClicked();
    void onCreateGroupFeedbackSignal(int,QString);
    void change(FriendInformation* uf);

signals:
    void createGroupRequestSignal(QString,QVector<QString>);
    void closeWindowSignal(QWidget*);
};

#endif // CREATEGROUPWINDOW_H
