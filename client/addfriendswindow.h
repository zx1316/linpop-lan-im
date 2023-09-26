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
    explicit AddFriendsWindow(const QString &selfName, MiHoYoLauncher *launcher, QWidget *parent = nullptr);
    ~AddFriendsWindow();
    void onAddFriendSuccess();
    void onAddFriendFail();
    void onAddFriendAlready();

private slots:
    void onSearchPushButtonClicked();

private:
    Ui::AddFriendsWindow *ui;
    MiHoYoLauncher *launcher;
    QString selfName;

signals:
    void addFriendRequestSignal(QString);
    void windowClosed();
};

#endif // ADDFRIENDS_H
