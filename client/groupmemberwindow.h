#ifndef GROUPMEMBERWINDOW_H
#define GROUPMEMBERWINDOW_H

#include <QWidget>

namespace Ui {
class GroupMemberWindow;
}

class GroupMemberWindow : public QWidget {
    Q_OBJECT

public:
    explicit GroupMemberWindow(QWidget *parent = nullptr);
    ~GroupMemberWindow();
    void onGroupMemberSignal(const QList<QString> &list);

private:
    Ui::GroupMemberWindow *ui;

private slots:
    void onRefreshClicked();

signals:
    void GroupMemberRequestSignal();
    void windowClosed();
};

#endif // GROUPMEMBERWINDOW_H
