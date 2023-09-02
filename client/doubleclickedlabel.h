#ifndef CBUTTON_H
#define CBUTTON_H
#include <QPushButton>
#include <QTimer>

class DoubleClickedLabel : public QPushButton
{
    Q_OBJECT
public:
    explicit DoubleClickedLabel(QWidget *pParent);

private:
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void slotTimerTimeOut();

private:
    QTimer m_cTimer;
signals:
    void doubleClickedSignal();
};

#endif // CBUTTON_H
