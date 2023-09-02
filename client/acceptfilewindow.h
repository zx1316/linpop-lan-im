#ifndef ACCEPTFILEWINDOW_H
#define ACCEPTFILEWINDOW_H

#include <QWidget>

namespace Ui {
class acceptFileWindow;
}

class acceptFileWindow : public QWidget
{
    Q_OBJECT

public:
    explicit acceptFileWindow(QWidget *parent = nullptr);
    ~acceptFileWindow();

private:
    Ui::acceptFileWindow *ui;
};

#endif // ACCEPTFILEWINDOW_H
