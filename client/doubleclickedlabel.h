#ifndef CBUTTON_H
#define CBUTTON_H
#include <QPushButton>
#include <QTimer>

class DoubleClickedLabel : public QPushButton {
    Q_OBJECT
public:
    explicit DoubleClickedLabel(QWidget *pParent);

private:
    QTimer m_cTimer;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void slotTimerTimeOut();

signals:
    void doubleClickedSignal();
};

#endif // CBUTTON_H
