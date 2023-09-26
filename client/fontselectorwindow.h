#ifndef FONTSELECTORWINDOW_H
#define FONTSELECTORWINDOW_H

#include <QWidget>

namespace Ui {
class FontSelectorWindow;
}

class FontSelectorWindow : public QWidget {
    Q_OBJECT

public:
    explicit FontSelectorWindow(const QFont &font, const QColor &color, QWidget *parent = nullptr);
    ~FontSelectorWindow();

private:
    Ui::FontSelectorWindow *ui;
    QColor color;

private slots:
    void onColorButtonClicked();
    void onOkButtonClicked();

signals:
    void updateFont(QFont font, QColor color);
    void windowClosed();
};

#endif // FONTSELECTORWINDOW_H
