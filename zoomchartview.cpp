/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "zoomchartview.h"
#include <QtGui/QMouseEvent>
#include <QDebug>
ZoomChartView::ZoomChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    m_leftbutton(false)
{
    m_zoomrange = 0;
//    setRubberBand(QChartView::RectangleRubberBand);
}

//![1]
//!

ZoomChartView::~ZoomChartView()
{
//    delete this;
}
void ZoomChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        chart()->scroll(-10, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(10, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 10);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -10);
        break;
//    case Qt::Key_Space:
//        chart()->zoomReset();
//        m_zoomrange =0;
//        break;
    default:
//        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ZoomChartView::wheelEvent(QWheelEvent *event)
{

    if( event->delta() > 0){                    // 当滚轮远离使用者时
        // chart()->zoomIn();                // 进行放大
        if(m_zoomrange<=5){
            chart()->zoomIn();
            m_zoomrange++;
        }
     }else{                                     // 当滚轮向使用者方向旋转时
        if(m_zoomrange>0){
            chart()->zoomOut();
            m_zoomrange--;
        }
     }
}

//void ZoomChartView::mouseMoveEvent(QMouseEvent *event){
//    if(m_leftbutton){
////        qDebug() << event->x() << event->y();
//    }
//}
void ZoomChartView::mousePressEvent(QMouseEvent *event){
    switch (event->button()) {
     case Qt::LeftButton:
        m_leftbutton=true;
        m_startPos = event->pos();
//        qDebug() << m_leftbutton;
        break;
    case Qt::RightButton:
        chart()->zoomReset();
        m_zoomrange =0;
       break;
    default:
        m_leftbutton=false;
       break;
    }
}

void ZoomChartView::mouseReleaseEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        m_leftbutton=false;
        m_endPos = event->pos();
        qreal dx = m_endPos.x()-m_startPos.x();
        qreal dy = m_endPos.y()-m_startPos.y();
        chart()->scroll(-dx,dy);
//        qDebug() << m_leftbutton;
    }
}

