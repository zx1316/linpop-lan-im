#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QWidget>

namespace Ui {
class AddFriends;
}

class AddFriends : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriends(QWidget *parent = nullptr);
    ~AddFriends();

private:
    Ui::AddFriends *ui;
};

#endif // ADDFRIENDS_H
