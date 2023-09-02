#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class AddFriendsWindow;
}

class AddFriendsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendsWindow(QWidget *parent = nullptr);
    ~AddFriendsWindow();

private slots:
    void onSearchPushButtonClicked();
public slots:
    void onAddFriendFeedback(int);

private:
    Ui::AddFriendsWindow *ui;
protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void addFriendRequestSignal(bool,QString);
    void closeWindowSignal(QWidget* w);
};

#endif // ADDFRIENDS_H
