#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>
#include "network.h"

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QWidget {
    Q_OBJECT

public:
    explicit ConfigWindow(Network *network, QWidget *parent = nullptr);
    ~ConfigWindow();

private:
    Ui::ConfigWindow *ui;
    Network *network;

private slots:
    void onConfirmClicked();
    void onClearCacheClicked();
};

#endif // CONFIGWINDOW_H
