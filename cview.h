#ifndef CVIEW_H
#define CVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QMenu>
#include <QtGui>

class CView : public QGraphicsView
{
    Q_OBJECT

signals:
    void mouseMoved( QMouseEvent *ev );
    void mouseReleased( QMouseEvent *ev );
    void wheelScrolled( QWheelEvent *ev );

public:
    CView(QWidget *parent = 0);
    void resetSize();

protected slots:
    void showContextMenu(const QPoint& pos);

protected:
    void enterEvent(QEvent *ev) {
        Q_UNUSED(ev);
        this->viewport()->setCursor(Qt::ArrowCursor);
    }
    void mouseMoveEvent( QMouseEvent *ev ) {
        emit mouseMoved( ev );
        QGraphicsView::mouseMoveEvent(ev);
    }
    void mouseReleaseEvent(QMouseEvent *ev) {
        emit mouseReleased( ev );
        QGraphicsView::mouseReleaseEvent(ev);
        this->viewport()->setCursor(Qt::ArrowCursor);
    }
    void wheelEvent( QWheelEvent *ev );


};


#endif // CVIEW_H
