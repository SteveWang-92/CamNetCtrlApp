#ifndef MOUSEMOVEGRAPHICSVIEW_H
#define MOUSEMOVEGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>

class MouseMoveGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MouseMoveGraphicsView(QWidget *parent = nullptr);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:


private:
    bool m_leftbutton;
};

#endif // MOUSEMOVEGRAPHICSVIEW_H
