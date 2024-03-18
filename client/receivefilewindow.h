#ifndef RECEIVEFILEWINDOW_H
#define RECEIVEFILEWINDOW_H

#include <QWidget>
#include "serverrecvthread.h"

namespace Ui {
class ReceiveFileWindow;
}

class ReceiveFileWindow : public QWidget {
    Q_OBJECT

public:
    ReceiveFileWindow(const QString& path, const QString& sender, const QString& ip, QWidget *parent = nullptr);
    ~ReceiveFileWindow();

public slots:
    void onFail();
    void onFinish();

private:
    Ui::ReceiveFileWindow *ui;
    ServerRecvThread *thread = nullptr;

signals:
    void accept(const QString&, int);
};

#endif // RECEIVEFILEWINDOW_H
