#include "mousemovegraphicsview.h"
#include <QMouseEvent>
#include <QDebug>
MouseMoveGraphicsView::MouseMoveGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    m_leftbutton= false;
}
void MouseMoveGraphicsView::mouseMoveEvent(QMouseEvent *event){
    if(m_leftbutton){
        qDebug() << event->x() << event->y();
    }
}
void MouseMoveGraphicsView::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_leftbutton=true;
         qDebug() << m_leftbutton;
    }
    else {
        m_leftbutton=false;
    }
}
void MouseMoveGraphicsView::mouseReleaseEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_leftbutton=false;
        qDebug() << m_leftbutton;
    }
}
