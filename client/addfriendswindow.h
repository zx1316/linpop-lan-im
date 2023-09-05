#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QWidget>
#include <QMessageBox>
#include "mihoyolauncher.h"

namespace Ui {
class AddFriendsWindow;
}

class AddFriendsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendsWindow(MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~AddFriendsWindow();

private slots:
    void onSearchPushButtonClicked();
public slots:
    void onAddFriendFeedback(int);

private:
    Ui::AddFriendsWindow *ui;
    MiHoYoLauncher *launcher;
protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void addFriendRequestSignal(bool,QString);
    void closeWindowSignal(QWidget* w);
};

#endif // ADDFRIENDS_H
