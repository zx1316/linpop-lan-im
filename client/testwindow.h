#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QWidget>
#include "receivefilewindow.h"
#include "transferfilewindow.h"


namespace Ui {
class TestWindow;
}

class TestWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TestWindow(QWidget *parent = nullptr);
    ~TestWindow();

private slots:
    void on_toReceiveWindow_clicked();

    void on_toSendWindow_clicked();

private:
    Ui::TestWindow *ui;
};

#endif // TESTWINDOW_H
