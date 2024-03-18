#ifndef CREATEGROUPWINDOW_H
#define CREATEGROUPWINDOW_H

#include <QWidget>
#include "mihoyolauncher.h"

namespace Ui {
class CreateGroupWindow;
}

class CreateGroupWindow : public QWidget {
    Q_OBJECT

public:
    CreateGroupWindow(const QList<QString>& friendList, const QString& selfName, MiHoYoLauncher *, QWidget *parent = nullptr);
    ~CreateGroupWindow();
    void onCreateGroupSuccessSignal();
    void onCreateGroupFailSignal();

private:
    Ui::CreateGroupWindow *ui;
    MiHoYoLauncher *launcher;
    QString selfName;

private slots:
    void onCreateGroupButtonClicked();
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onSelectImgButtonClicked();

signals:
    void createGroupRequestSignal(const QString& groupName, const QString& imgName, const QList<QString>&);
    void windowClosed();
};

#endif // CREATEGROUPWINDOW_H
