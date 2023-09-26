#include "doubleclickedlabel.h"

 DoubleClickedLabel:: DoubleClickedLabel(QWidget *pParent):QPushButton(pParent)
{
    connect(&m_cTimer,SIGNAL(timeout()),this,SLOT(slotTimerTimeOut()));
    this->setFlat(true);
    this->setStyleSheet("DoubleClickedLabel{border:none;background:transparent;}");
}

void  DoubleClickedLabel::mousePressEvent(QMouseEvent *e)
{
    m_cTimer.start(200);
}

void  DoubleClickedLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_cTimer.stop();
    emit doubleClickedSignal();
}

void  DoubleClickedLabel::slotTimerTimeOut()
{
    m_cTimer.stop();
}

