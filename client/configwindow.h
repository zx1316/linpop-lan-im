#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent, QString &ip, int *port);
    ~ConfigWindow();

private:
    Ui::ConfigWindow *ui;
    QString &ip;
    int *port;

public slots:
    void onConfirmClicked();
};

#endif // CONFIGWINDOW_H
